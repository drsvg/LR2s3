#include "queue.h"

void queueInit(Queue& q, const string& name) {
    q.name = name;
    q.front = nullptr;
    q.rear = nullptr;
    q.size = 0;
}

void queueClear(Queue& q) {
    QueueNode* cur = q.front;
    while (cur) {
        QueueNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    q.front = nullptr;
    q.rear = nullptr;
    q.size = 0;
}

CommandResult queuePush(Queue& q, const string& value) {
    QueueNode* node = new QueueNode;
    node->value = value;
    node->next = nullptr;
    if (q.rear) q.rear->next = node;
    else q.front = node;
    q.rear = node;
    q.size++;
    return makeOk(value);
}

CommandResult queuePop(Queue& q) {
    if (!q.front) return makeError("ERROR: queue is empty");
    QueueNode* del = q.front;
    string v = del->value;
    q.front = del->next;
    if (!q.front) q.rear = nullptr;
    delete del;
    q.size--;
    return makeOk(v);
}

CommandResult queuePrint(const Queue& q) {
    ostringstream oss;
    oss << "[";
    QueueNode* cur = q.front;
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