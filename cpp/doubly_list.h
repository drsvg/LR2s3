#ifndef DOUBLY_LIST_H
#define DOUBLY_LIST_H

#include "common.h"

struct DNode {
    string value;
    DNode* prev;
    DNode* next;
};

struct DoublyList {
    string name;
    DNode* head;
    DNode* tail;
    int size;
};

void doublyInit(DoublyList& l, const string& name);
void doublyClear(DoublyList& l);

CommandResult doublyPushHead(DoublyList& l, const string& value);
CommandResult doublyPushTail(DoublyList& l, const string& value);
CommandResult doublyPushAfter(DoublyList& l, const string& value, const string& target);
CommandResult doublyPushBefore(DoublyList& l, const string& value, const string& target);

CommandResult doublyDelHead(DoublyList& l);
CommandResult doublyDelTail(DoublyList& l);
CommandResult doublyDelValue(DoublyList& l, const string& value);
CommandResult doublyDelNear(DoublyList& l, const string& target, bool isAfter);

CommandResult doublyGet(const DoublyList& l);
CommandResult doublyFind(const DoublyList& l, const string& value);
CommandResult doublyPrint(const DoublyList& l);

#endif
