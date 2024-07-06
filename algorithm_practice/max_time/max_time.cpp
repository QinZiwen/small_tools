#include <iostream>

void findTime(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    const int timeTemp[] = {2, 3, 5, 9, 5, 9};
    std::string time;
    for (int i = 0; i < 6; ++i) {
        // std::cout << " >>>>>>>>>> " << i << " : " << timeTemp[i] << " <<<<<<<<<<<" << std::endl;
        size_t timeLen = time.length();
        for (int needTime = timeTemp[i]; needTime >= 0; --needTime) {
            for (int j = 0; j < n; ++j) {
                // std::cout << "arr: " << j << " " << arr[j] << std::endl;
                if (arr[j] >= 0 && arr[j] == needTime) {
                    time += std::to_string(arr[j]);
                    arr[j] = -1;
                    break;
                }
            }

            if (time.length() > timeLen) {
                break;
            }
        }

        if (i == 1 || i== 3) {
            time += ":";
        }
        // std::cout << "i: " << i << " -> " << time << std::endl;
    }

    if (time.length() == 8) {
        std::cout << time << std::endl;
    } else {
        std::cout << "invalid" << std::endl;
    }
}

int main() {
    {
        int time[6] = {0, 2, 3, 5, 3, 0};
        findTime(time, 6);
    }
    {
        int time[6] = {0, 1, 3, 9, 3, 0};
        findTime(time, 6);
    }
    {
        int time[6] = {0, 9, 3, 5, 3, 0};
        findTime(time, 6);
    }
    {
        int time[6] = {0, 0, 3, 5, 3, 0};
        findTime(time, 6);
    }
    {
        int time[6] = {9, 9, 9, 9, 9, 9};
        findTime(time, 6);
    }
    {
        int time[6] = {0, 0, 0, 0, 0, 0};
        findTime(time, 6);
    }
    {
        int time[6] = {3, 2, 9, 5, 5, 9};
        findTime(time, 6);
    }
    return 0;
}

// int main() {
//     Eigen::MatrixXd A = Eigen::MatrixXd::Random(3, 3);
//     return 0;
// }