#ifndef LRUCACHE_H
#define LRUCACHE_H

#include "common.h"

struct LRUNode {
    string key;
    string value;
    LRUNode* prev;
    LRUNode* next;
};

struct LRUHashEntry {
    string key;
    LRUNode* node;
    LRUHashEntry* next;
};

struct LRUCache {
    int capacity;
    int size;
    LRUNode* head;              
    LRUNode* tail;            
    LRUHashEntry** buckets;     
    int numBuckets;
};

void lruInit(LRUCache& c, int capacity);
void lruClear(LRUCache& c);

CommandResult lruSet(LRUCache& c, const string& key, const string& value);
CommandResult lruGet(LRUCache& c, const string& key);
CommandResult lruPrint(const LRUCache& c);  

#endif