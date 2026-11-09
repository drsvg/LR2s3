#ifndef SET_H
#define SET_H

#include "common.h"

struct SetNode {
    string value;
    SetNode* next;
};

const int SET_BUCKETS = 1021;

struct Set {
    string name;
    SetNode* buckets[SET_BUCKETS];
    int size;
};

void setInit(Set& s, const string& name);
void setClear(Set& s);

CommandResult setAdd(Set& s, const string& value);       
CommandResult setDel(Set& s, const string& value);      
CommandResult setAt(const Set& s, const string& value);  

CommandResult setPrint(const Set& s);  

#endif