#include "split.h"
#include <algorithm>
#include <vector>

static bool backtrack(const vector<int>& nums, int idx,
                      vector<int>& sums,
                      vector<vector<int>>& bins,
                      int target) {
    if (idx == (int)nums.size()) {
        for (int s : sums) if (s != target) return false;
        return true;
    }

    int v = nums[idx];
    for (int i = 0; i < (int)sums.size(); ++i) {
        if (sums[i] + v > target) continue;  

        sums[i] += v;
        bins[i].push_back(v);

        if (backtrack(nums, idx + 1, sums, bins, target)) return true;

        sums[i] -= v;
        bins[i].pop_back();

        if (sums[i] == 0) break;

        if (sums[i] + v == target) break;
    }
    return false;
}

CommandResult splitSet(const Set& s, int target) {
    if (target <= 0) return makeError("ERROR: target must be positive");

    vector<int> nums;
    int total = 0;
    for (int i = 0; i < SET_BUCKETS; ++i) {
        for (SetNode* cur = s.buckets[i]; cur; cur = cur->next) {
            int v;
            try {
                v = stoi(cur->value);
            } catch (...) {
                return makeError("ERROR: non-integer value in set");
            }
            if (v <= 0) return makeError("ERROR: set must contain positive naturals");
            nums.push_back(v);
            total += v;
        }
    }

    if (nums.empty()) return makeError("ERROR: set is empty");
    if (total % target != 0) {
        return makeError("ERROR: sum is not divisible by target");
    }

    int k = total / target;  
    if (k > (int)nums.size()) {
        return makeError("ERROR: too many subsets for this set size");
    }

    sort(nums.begin(), nums.end(), greater<int>());

    if (nums[0] > target) {
        return makeError("ERROR: element exceeds target sum");
    }

    vector<int> sums(k, 0);
    vector<vector<int>> bins(k);

    if (!backtrack(nums, 0, sums, bins, target)) {
        return makeError("ERROR: impossible to split");
    }

    ostringstream oss;
    for (int i = 0; i < k; ++i) {
        if (i > 0) oss << " ";
        oss << "{";
        for (int j = 0; j < (int)bins[i].size(); ++j) {
            if (j > 0) oss << ",";
            oss << bins[i][j];
        }
        oss << "}";
    }
    return makeOk(oss.str());
}