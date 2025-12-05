/*
 * @lc app=leetcode.cn id=56 lang=cpp
 *
 * [56] 合并区间
 */

// @lc code=start
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        vector<vector<int>> res;
        if (intervals.size() == 0) return res;
        sort(intervals.begin(), intervals.end());

        int start = intervals[0][0];
        int end = intervals[0][1];
        for (int i = 1; i < intervals.size(); ++i) {
            bool startIn = intervals[i][0] >= start && intervals[i][0] <= end;
            bool endIn = intervals[i][1] >= start && intervals[i][1] <= end;
            if (startIn || endIn) {
                start = min(start, intervals[i][0]);
                end = max(end, intervals[i][1]);
            } else {
                res.push_back({start, end});
                start = intervals[i][0];
                end = intervals[i][1];
            }
        }
        res.push_back({start, end});
        
        return res;
    }
};
// @lc code=end

