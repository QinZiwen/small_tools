#include <iostream>
#include <vector>
#include <list>

using namespace std;

class Solution {
public:
    vector<pair<size_t, size_t>> getAround(size_t i, size_t j, size_t h, size_t w) {
        vector<pair<size_t, size_t>> res;
        if (i > 0 && i - 1 >= 0) {
            res.emplace_back(make_pair(i - 1, j));
        }
        if (i + 1 < h) {
            res.emplace_back(make_pair(i + 1, j));
        }
        if (j > 0 && j - 1 >= 0) {
            res.emplace_back(make_pair(i, j - 1));
        }
        if (j + 1 < w) {
            res.emplace_back(make_pair(i, j + 1));
        }
        return res;
    }

    void printGrid(vector<vector<int>>& grid) {
        cout << "grid:" << endl;
        for (size_t i = 0; i < grid.size(); ++i) {
            for (size_t j = 0; j < grid[i].size(); ++j) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    int countSubIslands(vector<vector<int>>& grid1, vector<vector<int>>& grid2) {
        // 打印grid1
        cout << "grid1:" << endl;
        for (size_t i = 0; i < grid1.size(); ++i) {
            for (size_t j = 0; j < grid1[i].size(); ++j) {
                cout << grid1[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "grid2:" << endl;
        for (size_t i = 0; i < grid2.size(); ++i) {
            for (size_t j = 0; j < grid2[i].size(); ++j) {
                cout << grid2[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;

        // 1. 直接将两个grid相加，得到交集。grid1岛屿用1表示，grid2岛屿用2表示
        // 2. 统计3表示的岛屿个数
        // 有几个约束条件：1和3连通，2和3连通，1和2不连通，1和3表示的小岛中不能有2的出现
        vector<vector<int>> grid;
        for (size_t i = 0; i < grid1.size(); ++i) {
            vector<int> temp(grid1[i].size(), 0);
            for (size_t j = 0; j < grid1[i].size(); ++j) {
                temp[j] = grid1[i][j] + (grid2[i][j] > 0 ? 2 : 0);
            }
            grid.emplace_back(temp);
        }

        // 打印grid
        cout << "grid:" << endl;
        for (size_t i = 0; i < grid.size(); ++i) {
            for (size_t j = 0; j < grid[i].size(); ++j) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;

        size_t res = 0;
        for (size_t i = 0; i < grid.size(); ++i) {
            for (size_t j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == 3) {
                    bool subisland = true;
                    list<pair<size_t, size_t>> islands;
                    islands.emplace_back(make_pair(i, j));
                    cout << "find --->>>>> island: " << i << ", " << j << " size: " << islands.size() << endl;
                    while (!islands.empty()) {
                        cout << "islands size: " << islands.size() << endl;
                        // const pair<size_t, size_t>& island = islands.front();
                        pair<size_t, size_t> island = islands.front();
                        cout << "island: " << island.first << ", " << island.second << " : " << grid[island.first][island.second] << endl;
                            
                        islands.pop_front();
                        printGrid(grid);
                        cout << "island: " << island.first << ", " << island.second << " : " << grid[island.first][island.second] << endl;
                        if (grid[island.first][island.second] == 0) {
                            continue;
                        }

                        grid[island.first][island.second] = 0;
                        cout << "island: " << island.first << ", " << island.second << " size: " << islands.size() << endl;
                        vector<pair<size_t, size_t>> around = getAround(island.first, island.second, grid.size(), grid[i].size());
                        for (const pair<size_t, size_t>& p : around) {
                            cout << "around: " << p.first << ", " << p.second << endl;
                            if (grid[p.first][p.second] == 0 || grid[p.first][p.second] == 1) {
                                continue;
                            }
                            if (grid[p.first][p.second] == 2) {
                                subisland = false;
                                cout << "not subisland" << endl;
                                continue;
                            }
                            islands.emplace_back(p);
                            cout << "push: " << p.first << ", " << p.second << ", size: " << islands.size() << endl;
                            pair<size_t, size_t> island = islands.front();
                            cout << "island: " << island.first << ", " << island.second << " : " << grid[island.first][island.second] << endl;
                            island = islands.back();
                            cout << "back island: " << island.first << ", " << island.second << " : " << grid[island.first][island.second] << endl;
                        }
                        // if (!subisland) {
                        //     break;
                        // }
                    }
                    if (subisland) {
                        cout << "------------->> subisland <<-------------" << endl;
                        res += 1;
                    }
                }
            }
        }
        return res;
    }
};


int main() {
    Solution s;
    // vector<vector<int>> grid1 = {{1,1,1,0,0},{0,1,1,1,1},{0,0,0,0,0},{1,0,0,0,0},{1,1,0,1,1}};
    // vector<vector<int>> grid2 = {{1,1,1,0,0},{0,0,1,1,1},{0,1,0,0,0},{1,0,1,1,0},{0,1,0,1,0}};
    // vector<vector<int>> grid1 = {{1,1,1,0,0},{0,1,1,1,1},{0,0,0,0,0},{1,0,0,0,0},{1,1,0,1,1}};
    // vector<vector<int>> grid2 = {{1,1,1,0,0},{0,0,1,1,1},{0,1,0,0,0},{1,0,1,1,0},{0,1,0,1,0}};
    vector<vector<int>> grid1 = {{1,1,1,1,0,0},{1,1,0,1,0,0},{1,0,0,1,1,1},{1,1,1,0,0,1},{1,1,1,1,1,0},{1,0,1,0,1,0},{0,1,1,1,0,1},{1,0,0,0,1,1},{1,0,0,0,1,0},{1,1,1,1,1,0}};
    vector<vector<int>> grid2 = {{1,1,1,1,0,1},{0,0,1,0,1,0},{1,1,1,1,1,1},{0,1,1,1,1,1},{1,1,1,0,1,0},{0,1,1,1,1,1},{1,1,0,1,1,1},{1,0,0,1,0,1},{1,1,1,1,1,1},{1,0,0,1,0,0}};
    cout << s.countSubIslands(grid1, grid2) << endl;
    return 0;
}