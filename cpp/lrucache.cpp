#include "lrucache.h"

static unsigned long long hashKey(const string& s) {
    unsigned long long h = 1469598103934665603ULL;
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

static int bucketIndex(const LRUCache& c, const string& key) {
    return (int)(hashKey(key) % c.numBuckets);
}

static LRUHashEntry* hashFind(const LRUCache& c, const string& key) {
    int idx = bucketIndex(c, key);
    LRUHashEntry* e = c.buckets[idx];
    while (e) {
        if (e->key == key) return e;
        e = e->next;
    }
    return nullptr;
}

static void hashPut(LRUCache& c, const string& key, LRUNode* node) {
    int idx = bucketIndex(c, key);
    LRUHashEntry* e = c.buckets[idx];
    while (e) {
        if (e->key == key) {
            e->node = node; 
            return;
        }
        e = e->next;
    }

    LRUHashEntry* newE = new LRUHashEntry{key, node, c.buckets[idx]};
    c.buckets[idx] = newE;
}

static void hashRemove(LRUCache& c, const string& key) {
    int idx = bucketIndex(c, key);
    LRUHashEntry* e = c.buckets[idx];
    LRUHashEntry* prev = nullptr;
    while (e && e->key != key) { prev = e; e = e->next; }
    if (!e) return;
    if (prev) prev->next = e->next;
    else c.buckets[idx] = e->next;
    delete e;
}

static void moveToHead(LRUCache& c, LRUNode* node) {
    if (node == c.head) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == c.tail) c.tail = node->prev;

    node->prev = nullptr;
    node->next = c.head;
    if (c.head) c.head->prev = node;
    c.head = node;
    if (!c.tail) c.tail = node;
}

static string removeTail(LRUCache& c) {
    if (!c.tail) return "";
    LRUNode* del = c.tail;
    string key = del->key;

    c.tail = del->prev;
    if (c.tail) c.tail->next = nullptr;
    else c.head = nullptr; 

    delete del;
    return key;
}


void lruInit(LRUCache& c, int capacity) {
    if (capacity <= 0) capacity = 1;  
    c.capacity = capacity;
    c.size = 0;
    c.head = nullptr;
    c.tail = nullptr;
    c.numBuckets = capacity * 2 + 1; 
    if (c.numBuckets < 16) c.numBuckets = 16;
    c.buckets = new LRUHashEntry*[c.numBuckets];
    for (int i = 0; i < c.numBuckets; ++i) c.buckets[i] = nullptr;
}

void lruClear(LRUCache& c) {
    LRUNode* cur = c.head;
    while (cur) {
        LRUNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    if (c.buckets) {
        for (int i = 0; i < c.numBuckets; ++i) {
            LRUHashEntry* e = c.buckets[i];
            while (e) {
                LRUHashEntry* nxt = e->next;
                delete e;
                e = nxt;
            }
        }
        delete[] c.buckets;
        c.buckets = nullptr;
    }
    c.head = c.tail = nullptr;
    c.size = 0;
}


CommandResult lruSet(LRUCache& c, const string& key, const string& value) {
    LRUHashEntry* entry = hashFind(c, key);

    if (entry) {
        entry->node->value = value;
        moveToHead(c, entry->node);
        return makeOk(value);
    }

    if (c.size >= c.capacity) {
        string evictedKey = removeTail(c);
        hashRemove(c, evictedKey);
        c.size--;
    }

    LRUNode* node = new LRUNode{key, value, nullptr, c.head};
    if (c.head) c.head->prev = node;
    c.head = node;
    if (!c.tail) c.tail = node;

    hashPut(c, key, node);
    c.size++;
    return makeOk(value);
}


CommandResult lruGet(LRUCache& c, const string& key) {
    LRUHashEntry* entry = hashFind(c, key);
    if (!entry) {
        return makeOk("-1");
    }
    moveToHead(c, entry->node);
    return makeOk(entry->node->value);
}


CommandResult lruPrint(const LRUCache& c) {
    ostringstream oss;
    oss << "cap=" << c.capacity << " size=" << c.size << " [";
    bool first = true;
    for (LRUNode* cur = c.head; cur; cur = cur->next) {
        if (!first) oss << ", ";
        oss << cur->key << "->" << cur->value;
        first = false;
    }
    oss << "]";
    return makeOk(oss.str());
}