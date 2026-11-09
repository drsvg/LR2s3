#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

struct QueueNode {
    string value;
    QueueNode* next;
};

struct Queue {
    string name;
    QueueNode* front;
    QueueNode* rear;
    int size;
};

void queueInit(Queue& q, const string& name);
void queueClear(Queue& q);
CommandResult queuePush(Queue& q, const string& value);
CommandResult queuePop(Queue& q);
CommandResult queuePrint(const Queue& q);

#endif