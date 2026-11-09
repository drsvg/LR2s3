#include "singly_list.h"

void singlyInit(SinglyList& l, const string& name) {
    l.name = name;
    l.head = nullptr;
    l.tail = nullptr;
    l.size = 0;
}

void singlyClear(SinglyList& l) {
    SNode* cur = l.head;
    while (cur) {
        SNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    l.head = nullptr;
    l.tail = nullptr;
    l.size = 0;
}

CommandResult singlyPushHead(SinglyList& l, const string& value) {
    SNode* node = new SNode{value, l.head};
    l.head = node;
    if (!l.tail) l.tail = node;
    l.size++;
    return makeOk(value);
}

CommandResult singlyPushTail(SinglyList& l, const string& value) {
    SNode* node = new SNode{value, nullptr};
    if (l.tail) l.tail->next = node;
    else l.head = node;
    l.tail = node;
    l.size++;
    return makeOk(value);
}

CommandResult singlyPushAfter(SinglyList& l, const string& value, const string& target) {
    SNode* cur = l.head;
    while (cur && cur->value != target) cur = cur->next;
    if (!cur) return makeError("ERROR: target not found");
    
    SNode* node = new SNode{value, cur->next};
    cur->next = node;
    if (cur == l.tail) l.tail = node;
    l.size++;
    return makeOk(value);
}

CommandResult singlyPushBefore(SinglyList& l, const string& value, const string& target) {
    if (l.head && l.head->value == target) return singlyPushHead(l, value);
    
    SNode* cur = l.head;
    while (cur && cur->next && cur->next->value != target) cur = cur->next;
    if (!cur || !cur->next) return makeError("ERROR: target not found");
    
    SNode* node = new SNode{value, cur->next};
    cur->next = node;
    l.size++;
    return makeOk(value);
}


CommandResult singlyDelHead(SinglyList& l) {
    if (!l.head) return makeError("ERROR: list is empty");
    SNode* del = l.head;
    l.head = l.head->next;
    if (!l.head) l.tail = nullptr;
    string v = del->value;
    delete del;
    l.size--;
    return makeOk(v);
}

CommandResult singlyDelTail(SinglyList& l) {
    if (!l.head) return makeError("ERROR: list is empty");
    if (l.head == l.tail) return singlyDelHead(l);

    SNode* cur = l.head;
    while (cur->next != l.tail) cur = cur->next;
    
    string v = l.tail->value;
    delete l.tail;
    l.tail = cur;
    l.tail->next = nullptr;
    l.size--;
    return makeOk(v);
}

CommandResult singlyDelValue(SinglyList& l, const string& value) {
    if (!l.head) return makeError("ERROR: list is empty");
    if (l.head->value == value) return singlyDelHead(l);

    SNode* cur = l.head;
    while (cur->next && cur->next->value != value) cur = cur->next;
    if (!cur->next) return makeError("ERROR: value not found");
    
    SNode* del = cur->next;
    string v = del->value;
    cur->next = del->next;
    if (del == l.tail) l.tail = cur;
    delete del;
    l.size--;
    return makeOk(v);
}

CommandResult singlyDelNear(SinglyList& l, const string& target, bool isAfter) {
    if (!l.head) return makeError("ERROR: list is empty");
    if (!isAfter && l.head->value == target) {
        return makeError("ERROR: nothing to delete before head");
    }
    if (isAfter && l.tail->value == target) {
        return makeError("ERROR: nothing to delete after tail");
    }

    SNode* cur = l.head;
    while (cur && cur->value != target) cur = cur->next;
    if (!cur) return makeError("ERROR: target not found");

    if (isAfter) {
        SNode* del = cur->next;
        string v = del->value;
        cur->next = del->next;
        if (del == l.tail) l.tail = cur;
        delete del;
        l.size--;
        return makeOk(v);
    } else {
        SNode* prev = l.head;
        while (prev && prev->next != cur) prev = prev->next;
        
        SNode* del = cur; 
        if (prev == l.head) {
            return singlyDelHead(l);
        }
        
        SNode* prevPrev = l.head;
        while (prevPrev && prevPrev->next != prev) prevPrev = prevPrev->next;
        
        prevPrev->next = prev->next;
        string v = prev->value;
        delete prev;
        l.size--;
        return makeOk(v);
    }
}

CommandResult singlyGet(const SinglyList& l) {
    ostringstream oss;
    oss << "[";
    SNode* cur = l.head;
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

CommandResult singlyFind(const SinglyList& l, const string& value) {
    SNode* cur = l.head;
    while (cur) {
        if (cur->value == value) return makeOk("TRUE");
        cur = cur->next;
    }
    return makeOk("FALSE");
}

CommandResult singlyPrint(const SinglyList& l) {
    return singlyGet(l);
}
