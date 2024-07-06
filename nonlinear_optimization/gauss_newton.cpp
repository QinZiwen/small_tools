#include <iostream>
#include <random>
#include <chrono>
#include <Eigen/Core>
#include <Eigen/Dense>

void gaussNewton(bool useSigma) {
    double ar = 1.0, br = 2.0, cr = 1.0;
    double ae = 4.1, be = 3.9, ce = 2.1;
    int N = 100;
    double w_sigma = 1.32;
    double inv_sigma = 1.0 / std::pow(w_sigma, 2);
    std::cout << "guess params: " << ae << ", " << be << ", " << ce << std::endl;
    
    std::random_device rd {};
    std::mt19937 gen {rd()};
    std::normal_distribution<double> d {0.0, w_sigma * w_sigma};

    // generate data
    std::vector<double> x_data, y_data;
    for (int i = 0; i < N; ++i) {
        double x = i / 100.0;
        x_data.push_back(x);
        y_data.emplace_back(exp(ar * x * x + br * x + cr) + d(gen));
    }

    // iteration
    int iterations = 100;
    double cost = 0.0, last_cost = 0.0;

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (int iter = 0; iter < iterations; ++iter) {
        Eigen::Matrix3d H = Eigen::Matrix3d::Zero();
        Eigen::Vector3d b = Eigen::Vector3d::Zero();
        cost = 0.0;

        for (int i = 0; i < N; ++i) {
            double x = x_data[i], y = y_data[i];
            double error = y - exp(ae * x * x + be * x + ce);
            Eigen::Vector3d J;
            J[0] = -x * x * exp(ae * x * x + be * x + ce);
            J[1] = -x * exp(ae * x * x + be * x + ce);
            J[2] = -exp(ae * x * x + be * x + ce);

            if (useSigma) {
                H += inv_sigma * inv_sigma * J * J.transpose();
                b += -inv_sigma * inv_sigma * J * error;
            } else {
                H += J * J.transpose();
                b += -J * error;
            }

            cost += error * error;
        }

        Eigen::Vector3d dx = H.ldlt().solve(b);
        if (std::isnan(dx[0])) {
            std::cout << "result is nan!" << std::endl;
            break;
        }

        if (iter > 0 && cost >= last_cost) {
            std::cout << "cost: " << cost << ", last cost: " << last_cost << std::endl;
            break;
        }

        ae += dx[0];
        be += dx[1];
        ce += dx[2];
        last_cost = cost;
        std::cout << "iter: " << iter << ", total cost: " << cost << ", \t\t update: " << dx.transpose()
                  << ", \t\t estimated params: " << ae << ", " << be << ", " << ce << std::endl;
    }

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    double used_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "used time: " << used_time << "ms" << std::endl;
    std::cout << "estimated params: " << ae << ", " << be << ", " << ce << std::endl;
    std::cout << "real params: " << ar << ", " << br << ", " << cr << std::endl;
}

int main() {
    gaussNewton(true);
    gaussNewton(false);
    return 0;
}