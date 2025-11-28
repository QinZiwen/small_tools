/*
 * @lc app=leetcode.cn id=32 lang=cpp
 *
 * [32] 最长有效括号
 */

// @lc code=start
class Solution {
public:
    int longestValidParentheses(string s) {
        if (s.size() < 2)
            return 0;
        
        vector<int> dp(s.size() + 1);
        int maxLen = 0;
        for (size_t i = 2; i < s.size() + 1; ++i) {
            if (s[i - 1] == ')') {
                if (s[i - 2] == '(') {
                    dp[i] = dp[i - 2] + 2;
                } else {
                    int matchPos = i - 2 - dp[i - 1];
                    if (matchPos >= 0 && s[matchPos] == '(') {
                        dp[i] = dp[i - 1] + 2 + dp[matchPos];
                    }
                }
            }
            maxLen = max(maxLen, dp[i]);
        }

        return maxLen;
    }
};
// @lc code=end

