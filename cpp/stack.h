#ifndef STACK_H
#define STACK_H

#include "common.h"

struct StackNode {
    string value;
    StackNode* next;
};

struct Stack {
    string name;
    StackNode* top;
    int size;
};

void stackInit(Stack& s, const string& name);
void stackClear(Stack& s);
CommandResult stackPush(Stack& s, const string& value);
CommandResult stackPop(Stack& s);
CommandResult stackPrint(const Stack& s);

#endif