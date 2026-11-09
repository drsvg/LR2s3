#include "array.h"

void arrayInit(Array& a, const string& name) {
    a.name = name;
    a.size = 0;
}

void arrayClear(Array& a) {
    a.size = 0;
}

CommandResult arrayPush(Array& a, const string& value, int index) {
    if (a.size >= MAX_ARRAY_SIZE) {
        return makeError("ERROR: array overflow");
    }
    if (index < 0 || index > a.size) {
        if (index == -1) {
            a.data[a.size] = value;
            a.size++;
            return makeOk(value);
        }
        return makeError("ERROR: index out of range");
    }
    for (int i = a.size; i > index; --i) {
        a.data[i] = a.data[i - 1];
    }
    a.data[index] = value;
    a.size++;
    return makeOk(value);
}

CommandResult arrayDel(Array& a, int index) {
    if (index < 0 || index >= a.size) {
        return makeError("ERROR: index out of range");
    }
    string removed = a.data[index];
    for (int i = index; i < a.size - 1; ++i) {
        a.data[i] = a.data[i + 1];
    }
    a.size--;
    return makeOk(removed);
}

CommandResult arrayGet(const Array& a, int index) {
    if (index < 0 || index >= a.size) {
        return makeError("ERROR: index out of range");
    }
    return makeOk(a.data[index]);
}

CommandResult arraySet(Array& a, int index, const string& value) {
    if (index < 0 || index >= a.size) {
        return makeError("ERROR: index out of range");
    }
    a.data[index] = value;
    return makeOk(value);
}

CommandResult arrayLen(const Array& a) {
    return makeOk(to_string(a.size));
}

CommandResult arrayPrint(const Array& a) {
    ostringstream oss;
    oss << "[";
    for (int i = 0; i < a.size; ++i) {
        if (i > 0) oss << ", ";
        oss << a.data[i];
    }
    oss << "]";
    return makeOk(oss.str());
}