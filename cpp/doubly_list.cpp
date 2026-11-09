#include "doubly_list.h"

void doublyInit(DoublyList& l, const string& name) {
    l.name = name;
    l.head = nullptr;
    l.tail = nullptr;
    l.size = 0;
}

void doublyClear(DoublyList& l) {
    DNode* cur = l.head;
    while (cur) {
        DNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    l.head = nullptr;
    l.tail = nullptr;
    l.size = 0;
}

CommandResult doublyPushHead(DoublyList& l, const string& value) {
    DNode* node = new DNode{value, nullptr, l.head};
    if (l.head) l.head->prev = node;
    l.head = node;
    if (!l.tail) l.tail = node;
    l.size++;
    return makeOk(value);
}

CommandResult doublyPushTail(DoublyList& l, const string& value) {
    DNode* node = new DNode{value, l.tail, nullptr};
    if (l.tail) l.tail->next = node;
    l.tail = node;
    if (!l.head) l.head = node;
    l.size++;
    return makeOk(value);
}

CommandResult doublyPushAfter(DoublyList& l, const string& value, const string& target) {
    DNode* cur = l.head;
    while (cur && cur->value != target) cur = cur->next;
    if (!cur) return makeError("ERROR: target not found");

    DNode* node = new DNode{value, cur, cur->next};
    if (cur->next) cur->next->prev = node;
    else l.tail = node;
    cur->next = node;
    l.size++;
    return makeOk(value);
}

CommandResult doublyPushBefore(DoublyList& l, const string& value, const string& target) {
    DNode* cur = l.head;
    while (cur && cur->value != target) cur = cur->next;
    if (!cur) return makeError("ERROR: target not found");

    DNode* node = new DNode{value, cur->prev, cur};
    if (cur->prev) cur->prev->next = node;
    else l.head = node;
    cur->prev = node;
    l.size++;
    return makeOk(value);
}


CommandResult doublyDelHead(DoublyList& l) {
    if (!l.head) return makeError("ERROR: list is empty");
    DNode* del = l.head;
    l.head = l.head->next;
    if (l.head) l.head->prev = nullptr;
    else l.tail = nullptr;
    string v = del->value;
    delete del;
    l.size--;
    return makeOk(v);
}

CommandResult doublyDelTail(DoublyList& l) {
    if (!l.tail) return makeError("ERROR: list is empty");
    DNode* del = l.tail;
    l.tail = l.tail->prev;
    if (l.tail) l.tail->next = nullptr;
    else l.head = nullptr;
    string v = del->value;
    delete del;
    l.size--;
    return makeOk(v);
}

CommandResult doublyDelValue(DoublyList& l, const string& value) {
    DNode* cur = l.head;
    while (cur && cur->value != value) cur = cur->next;
    if (!cur) return makeError("ERROR: value not found");
    
    string v = cur->value;
    if (cur->prev) cur->prev->next = cur->next;
    else l.head = cur->next;
    if (cur->next) cur->next->prev = cur->prev;
    else l.tail = cur->prev;
    delete cur;
    l.size--;
    return makeOk(v);
}

CommandResult doublyDelNear(DoublyList& l, const string& target, bool isAfter) {
    DNode* cur = l.head;
    while (cur && cur->value != target) cur = cur->next;
    if (!cur) return makeError("ERROR: target not found");

    DNode* del = isAfter ? cur->next : cur->prev;
    if (!del) return makeError("ERROR: nothing to delete");

    string v = del->value;
    if (del->prev) del->prev->next = del->next;
    else l.head = del->next;
    if (del->next) del->next->prev = del->prev;
    else l.tail = del->prev;
    delete del;
    l.size--;
    return makeOk(v);
}

CommandResult doublyGet(const DoublyList& l) {
    ostringstream oss;
    oss << "[";
    DNode* cur = l.head;
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

CommandResult doublyFind(const DoublyList& l, const string& value) {
    DNode* cur = l.head;
    while (cur) {
        if (cur->value == value) return makeOk("TRUE");
        cur = cur->next;
    }
    return makeOk("FALSE");
}

CommandResult doublyPrint(const DoublyList& l) {
    return doublyGet(l);
}
