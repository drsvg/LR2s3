#include "stack.h"

void stackInit(Stack& s, const string& name) {
    s.name = name;
    s.top = nullptr;
    s.size = 0;
}

void stackClear(Stack& s) {
    StackNode* cur = s.top;
    while (cur) {
        StackNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    s.top = nullptr;
    s.size = 0;
}

CommandResult stackPush(Stack& s, const string& value) {
    StackNode* node = new StackNode;
    node->value = value;
    node->next = s.top;
    s.top = node;
    s.size++;
    return makeOk(value);
}

CommandResult stackPop(Stack& s) {
    if (!s.top) return makeError("ERROR: stack is empty");
    StackNode* del = s.top;
    string v = del->value;
    s.top = del->next;
    delete del;
    s.size--;
    return makeOk(v);
}

CommandResult stackPrint(const Stack& s) {
    ostringstream oss;
    oss << "[";
    StackNode* cur = s.top;
    bool first = true;
    while (cur) {
        if (!first) oss << ", ";
        oss << cur->value;
        first = false;
        cur = cur->next;
    }
    oss << "]";
    return makeOk(oss.str());
}