/*
 * @lc app=leetcode.cn id=322 lang=cpp
 *
 * [322] 零钱兑换
 */

// @lc code=start
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        if (amount == 0)
            return 0;
        if (coins.size() == 0)
            return -1;

        vector<int> dp(amount + 1, amount + 1);
        dp[0] = 0;

        for (size_t i = 0; i < coins.size(); ++i) {
            for (int j = 1; j <= amount; ++j) {
                if (j >= coins[i])
                    dp[j] = min(dp[j], dp[j - coins[i]] + 1);
            }
        }

        return dp[amount] == amount + 1 ? -1 : dp[amount];
    }
};
// @lc code=end

