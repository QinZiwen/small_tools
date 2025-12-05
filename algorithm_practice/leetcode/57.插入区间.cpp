/*
 * @lc app=leetcode.cn id=57 lang=cpp
 *
 * [57] 插入区间
 */

// @lc code=start
class Solution {
public:
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        vector<vector<int>> res;
        if (intervals.empty()) {
            res.push_back(newInterval);
            return res;
        }
        
        bool inserted = false;
        for (size_t i = 0; i < intervals.size(); ++i) {
            if (newInterval[1] < intervals[i][0]) {
                if (!inserted) {
                    res.push_back(newInterval);
                    inserted = true;
                }
                res.push_back(intervals[i]);
            } else if (newInterval[0] > intervals[i][1]) {
                res.push_back(intervals[i]);
                if (i + 1 == intervals.size()) {
                    res.push_back(newInterval);
                }
            } else {
                newInterval[0] = min(newInterval[0], intervals[i][0]);
                newInterval[1] = max(newInterval[1], intervals[i][1]);
                if (i + 1 == intervals.size()) {
                    res.push_back(newInterval);
                }
            }
        }
        return res;
    }
};
// @lc code=end

