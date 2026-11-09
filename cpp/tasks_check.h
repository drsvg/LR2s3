#ifndef TASKS_CHECK_H
#define TASKS_CHECK_H

#include "common.h"
#include <map>
#include <vector>

CommandResult checkTaskOrder(const vector<string>& tasks,
                             const vector<pair<string, string>>& dependencies);

#endif