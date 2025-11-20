/*
 * @lc app=leetcode.cn id=15 lang=cpp
 *
 * [15] 三数之和
 */

// @lc code=start
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        if (nums.size() < 3) {
            return {};
        }

        vector<vector<int>> result;
        std::sort(nums.begin(), nums.end());

        for (size_t start = 0; start + 2 < nums.size(); ++start) {
            if (nums[start] > 0) break;
            if (start > 0 && nums[start] == nums[start - 1]) {
                continue;
            }

            if (nums[start] + nums[start + 1] + nums[start + 2] > 0) break;
            if (nums[start] + nums[nums.size() - 2] + nums[nums.size() - 1] < 0) continue;

            size_t left = start + 1, right = nums.size() - 1;
            while (left < right) { 
                int sum = nums[start] + nums[left] + nums[right];
                if (sum == 0) {
                    result.emplace_back(std::vector<int>{nums[start], nums[left], nums[right]});

                    size_t last = left;
                    while (++left && left < right && nums[left] == nums[last]) {}
                    last = right;
                    while (--right && left < right && nums[right] == nums[last]) {}
                } else if (sum < 0) {
                    ++left;
                } else {
                    --right;
                }
            }
        }

        return result;
    }
};
// @lc code=end

