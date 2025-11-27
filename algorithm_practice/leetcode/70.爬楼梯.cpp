/*
 * @lc app=leetcode.cn id=70 lang=cpp
 *
 * [70] 爬楼梯
 */

// @lc code=start
class Solution {
public:
    int climbStairs(int n) {
        if (n < 1)
            return 0;
        else if (n == 1)
            return 1;
        else if (n == 2)
            return 2;
        
        int step1 = 1, step2 = 2;
        int temp;
        for (int i = 3; i < n + 1; ++i) {
            temp = step1;
            step1 = step2;
            step2 = temp + step2;
        }

        return step2;
    }
};
// @lc code=end

