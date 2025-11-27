/*
 * @lc app=leetcode.cn id=213 lang=cpp
 *
 * [213] 打家劫舍 II
 */

// @lc code=start
class Solution {
public:
    int robBase(vector<int>& nums, int start, int end) {
        int pprev = 0;
        int prev = nums[start];
        for (int i = start + 1; i < end; ++i) {
            int temp = prev;
            prev = max(prev, pprev + nums[i]);
            pprev = temp;
        }
        
        return prev;
    }

    int rob(vector<int>& nums) {
        if (nums.size() == 0) {
            return 0;
        }
        if (nums.size() == 1) {
            return nums[0];
        }
        return max(robBase(nums, 0, nums.size() - 1), robBase(nums, 1, nums.size()));
    }
};
// @lc code=end

