#include <iostream>
#include <vector>

void move(std::vector<int>& src, std::vector<int>& tar) {
    int v = src.back();
    src.pop_back();
    tar.push_back(v);
}

void dfs(int i, std::vector<int>& src, std::vector<int>& buf, std::vector<int>& tar) {
    if (i == 1) {
        move(src, tar);
        return;
    }

    dfs(i - 1, src, tar, buf);
    move(src, tar);
    dfs(i - 1, buf, src, tar);
}

void solve_hanota(std::vector<int>& src, std::vector<int>& buf, std::vector<int>& tar)
{
    dfs(src.size(), src, buf, tar);
}

void print_vec(const std::vector<int>& vec) {
    for (int v : vec) {
        std::cout << " " << v;
    }
    std::cout << std::endl;
}
int main() {
    std::vector<int> src = {1,2,3,4,5,6,7,8,9};
    std::vector<int> buf, tar;

    solve_hanota(src, buf, tar);
    std::cout << "src:";
    print_vec(src);
    std::cout << "buf:";
    print_vec(buf);
    std::cout << "tar:";
    print_vec(tar);
    return 0;
}