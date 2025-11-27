/*
 * @lc app=leetcode.cn id=198 lang=cpp
 *
 * [198] 打家劫舍
 */

// @lc code=start
class Solution {
public:
    int rob(vector<int>& nums) {
        if (nums.size() == 0) {
            return 0;
        }

        if (nums.size() == 1) {
            return nums[0];
        }

        int pprev = 0;
        int prev = nums[0];
        for (int i = 1; i < nums.size(); ++i) {
            int temp = prev;
            prev = max(prev, pprev + nums[i]);
            pprev = temp;
        }
        
        return prev;
    }
};
// @lc code=end

