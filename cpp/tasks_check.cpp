#include "tasks_check.h"
#include "stack.h"
#include <map>
#include <vector>
#include <set>

CommandResult checkTaskOrder(const vector<string>& tasks,
                             const vector<pair<string, string>>& dependencies) {
   
    map<string, int> indeg;
    map<string, vector<string>> adj;
    set<string> known; 

    for (const string& t : tasks) {
        indeg[t] = 0;
        known.insert(t);
    }

    for (const auto& edge : dependencies) {
        const string& task = edge.first;
        const string& dep  = edge.second;

        if (!known.count(task) || !known.count(dep)) {
            return makeError("ERROR: unknown task name in dependencies");
        }
        adj[dep].push_back(task);
        indeg[task]++;
    }

    Stack stk;
    stackInit(stk, "check_stack");
    for (const string& t : tasks) {
        if (indeg[t] == 0) {
            stackPush(stk, t);
        }
    }

    int done = 0;
    int total = (int)tasks.size();
    while (stk.size > 0) {
        CommandResult r = stackPop(stk);
        if (!r.ok) break;
        string cur = r.output;
        done++;

        for (const string& nxt : adj[cur]) {
            indeg[nxt]--;
            if (indeg[nxt] == 0) {
                stackPush(stk, nxt);
            }
        }
    }

    stackClear(stk);

    return makeOk(done == total ? "TRUE" : "FALSE");
}