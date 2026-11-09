#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

const int MAX_ARRAY_SIZE = 10000;

struct Array {
    string name;
    string data[MAX_ARRAY_SIZE];
    int size;
};

void arrayInit(Array& a, const string& name);
CommandResult arrayPush(Array& a, const string& value, int index = -1);
CommandResult arrayDel(Array& a, int index);
CommandResult arrayGet(const Array& a, int index);
CommandResult arraySet(Array& a, int index, const string& value);
CommandResult arrayLen(const Array& a);
CommandResult arrayPrint(const Array& a);
void arrayClear(Array& a);

#endif