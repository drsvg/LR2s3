#ifndef SINGLY_LIST_H
#define SINGLY_LIST_H

#include "common.h"

struct SNode {
    string value;
    SNode* next;
};

struct SinglyList {
    string name;
    SNode* head;
    SNode* tail;
    int size;
};

void singlyInit(SinglyList& l, const string& name);
void singlyClear(SinglyList& l);

CommandResult singlyPushHead(SinglyList& l, const string& value);
CommandResult singlyPushTail(SinglyList& l, const string& value);
CommandResult singlyPushAfter(SinglyList& l, const string& value, const string& target);
CommandResult singlyPushBefore(SinglyList& l, const string& value, const string& target);

CommandResult singlyDelHead(SinglyList& l);
CommandResult singlyDelTail(SinglyList& l);
CommandResult singlyDelValue(SinglyList& l, const string& value);
CommandResult singlyDelNear(SinglyList& l, const string& target, bool isAfter); 

CommandResult singlyGet(const SinglyList& l);
CommandResult singlyFind(const SinglyList& l, const string& value);
CommandResult singlyPrint(const SinglyList& l);

#endif
