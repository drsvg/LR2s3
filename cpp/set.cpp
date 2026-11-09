#include "set.h"

void setInit(Set& s, const string& name) {
    s.name = name;
    s.size = 0;
    for (int i = 0; i < SET_BUCKETS; ++i) s.buckets[i] = nullptr;
}

void setClear(Set& s) {
    for (int i = 0; i < SET_BUCKETS; ++i) {
        SetNode* cur = s.buckets[i];
        while (cur) {
            SetNode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        s.buckets[i] = nullptr;
    }
    s.size = 0;
}

static int hashIdx(const string& v) {
    unsigned h = 0;
    for (unsigned char c : v) h = h * 31 + c;
    return (int)(h % SET_BUCKETS);
}

CommandResult setAdd(Set& s, const string& value) {
    int i = hashIdx(value);
    for (SetNode* cur = s.buckets[i]; cur; cur = cur->next) {
        if (cur->value == value) return makeOk(value); 
    s.buckets[i] = new SetNode{value, s.buckets[i]};
    s.size++;
    return makeOk(value);
}

CommandResult setDel(Set& s, const string& value) {
    int i = hashIdx(value);
    SetNode* cur = s.buckets[i];
    SetNode* prev = nullptr;
    while (cur && cur->value != value) { prev = cur; cur = cur->next; }
    if (!cur) return makeError("ERROR: value not found");

    if (prev) prev->next = cur->next;
    else s.buckets[i] = cur->next;
    delete cur;
    s.size--;
    return makeOk(value);
}

CommandResult setAt(const Set& s, const string& value) {
    int i = hashIdx(value);
    for (SetNode* cur = s.buckets[i]; cur; cur = cur->next) {
        if (cur->value == value) return makeOk("TRUE");
    }
    return makeOk("FALSE");
}

CommandResult setPrint(const Set& s) {
    ostringstream oss;
    oss << "{";
    bool first = true;
    for (int i = 0; i < SET_BUCKETS; ++i) {
        for (SetNode* cur = s.buckets[i]; cur; cur = cur->next) {
            if (!first) oss << ", ";
            oss << cur->value;
            first = false;
        }
    }
    oss << "}";
    return makeOk(oss.str());
}