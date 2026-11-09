#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

enum ContainerType {
    CT_ARRAY,
    CT_SINGLY,
    CT_DOUBLY,
    CT_STACK,
    CT_QUEUE,
    CT_TREE,
    CT_UNKNOWN
};

struct CommandResult {
    bool ok;
    string output;
};

inline CommandResult makeOk(const string& out = "") {
    CommandResult r;
    r.ok = true;
    r.output = out;
    return r;
}

inline CommandResult makeError(const string& msg) {
    CommandResult r;
    r.ok = false;
    r.output = msg;
    return r;
}

#endif