#include "subsets.h"
#include <vector>
#include <algorithm>

CommandResult printAllSubsets(const Array& a) {
    if (a.size == 0) {
        return makeOk("[{}]");
    }

    if (a.size > 20) {
        return makeError("ERROR: array too large for subsets (max 20 elements)");
    }

    Set uniqueSubsets;
    setInit(uniqueSubsets, "temp_subsets");

    unsigned long long totalMasks = 1ULL << a.size;

    for (unsigned long long mask = 0; mask < totalMasks; ++mask) {
        ostringstream oss;
        oss << "{";
        bool first = true;

        for (int i = 0; i < a.size; ++i) {
            if (mask & (1ULL << i)) {
                if (!first) oss << ", ";
                oss << a.data[i];
                first = false;
            }
        }
        oss << "}";

        setAdd(uniqueSubsets, oss.str());
    }

    vector<string> result;
    for (int i = 0; i < SET_BUCKETS; ++i) {
        for (SetNode* cur = uniqueSubsets.buckets[i]; cur; cur = cur->next) {
            result.push_back(cur->value);
        }
    }
    sort(result.begin(), result.end(), [](const string& a, const string& b) {
        if (a.length() != b.length()) return a.length() < b.length();
        return a < b;
    });

    ostringstream finalOss;
    finalOss << "[";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) finalOss << ", ";
        finalOss << result[i];
    }
    finalOss << "]";

    setClear(uniqueSubsets); 
    return makeOk(finalOss.str());
}