#include "ceres/ceres.h"

#include <chrono>
#include <iostream>
#include <random>

struct CurveFittingCostFunction {
    const double x, y;
    CurveFittingCostFunction(double x_, double y_) : x(x_), y(y_) {}

    template <typename T>
    bool operator()(const T* const abc, T* residual) const {
        residual[0] = y - ceres::exp(abc[0] * T(x) * T(x) + abc[1] * T(x) + abc[2]);
        return true;
    }
};

void curveFitting(bool useSigma) {
    double ar = 1.0, br = 2.0, cr = 1.0;
    double ae = 4.1, be = 3.9, ce = 2.1;
    int N            = 100;
    double w_sigma   = 1.32;
    double inv_sigma = 1.0 / std::pow(w_sigma, 2);
    std::cout << "guess params: " << ae << ", " << be << ", " << ce << std::endl;

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> d{0.0, w_sigma * w_sigma};

    // generate data
    std::vector<double> x_data, y_data;
    for (int i = 0; i < N; ++i) {
        double x = i / 100.0;
        x_data.push_back(x);
        y_data.emplace_back(exp(ar * x * x + br * x + cr) + d(gen));
    }

    double abc[3] = {ae, be, ce};
    ceres::Problem problem;
    for (int i = 0; i < N; ++i) {
        problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CurveFittingCostFunction, 1, 3>(
                                     new CurveFittingCostFunction(x_data[i], y_data[i])),
                                 nullptr, abc);
    }

    ceres::Solver::Options options;
    options.linear_solver_type           = ceres::DENSE_NORMAL_CHOLESKY;
    options.minimizer_progress_to_stdout = true;

    ceres::Solver::Summary summary;
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    ceres::Solve(options, &problem, &summary);
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    double used_time                         = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "used time: " << used_time << "ms" << std::endl;
    std::cout << "origin params: " << ar << ", " << br << ", " << cr << std::endl;
    std::cout << "estimated params: " << abc[0] << ", " << abc[1] << ", " << abc[2] << std::endl;
    std::cout << "BriefReport: " << summary.BriefReport() << std::endl;
}

int main() {
    curveFitting(true);
    return 0;
}