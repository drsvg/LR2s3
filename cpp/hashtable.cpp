#include "hashtable.h"

static unsigned long long h1(const string& s) {
    unsigned long long h = 1469598103934665603ULL;
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

static int h2(const string& s, int capacity) {
    unsigned long long h = 0;
    for (unsigned char c : s) h = h * 131 + c;
    return 1 + (int)(h % (capacity - 1));
}

void htInit(HashTable& h, const string& name, ProbeMode mode, int capacity) {
    h.name = name;
    h.mode = mode;
    h.capacity = capacity;
    h.size = 0;
    h.deletedCount = 0;
    h.slots = new Slot[capacity];
    for (int i = 0; i < capacity; ++i) h.slots[i].state = SLOT_EMPTY;
}

void htClear(HashTable& h) {
    if (h.slots) {
        delete[] h.slots;
        h.slots = nullptr;
    }
    h.capacity = 0;
    h.size = 0;
    h.deletedCount = 0;
}

static int probeIndex(const HashTable& h, const string& value, int i) {
    int base = (int)(h1(value) % h.capacity);
    if (h.mode == PROBE_LINEAR) {
        return (base + i) % h.capacity;
    } else { 
        int step = h2(value, h.capacity);
        return (int)((base + 1ULL * i * step) % h.capacity);
    }
}


static void rehash(HashTable& h) {
    int newCap = h.capacity; 
    Slot* oldSlots = h.slots;
    int oldCap = h.capacity;

    h.slots = new Slot[newCap];
    for (int i = 0; i < newCap; ++i) h.slots[i].state = SLOT_EMPTY;
    h.size = 0;
    h.deletedCount = 0;

    for (int i = 0; i < oldCap; ++i) {
        if (oldSlots[i].state == SLOT_OCCUPIED) {
            htInsert(h, oldSlots[i].key);
        }
    }
    delete[] oldSlots;
}


CommandResult htInsert(HashTable& h, const string& value) {
    int firstDeleted = -1;

    for (int i = 0; i < h.capacity; ++i) {
        int idx = probeIndex(h, value, i);
        if (h.slots[idx].state == SLOT_OCCUPIED && h.slots[idx].key == value) {
            return makeOk(value);   
        }
        if (h.slots[idx].state == SLOT_DELETED && firstDeleted == -1) {
            firstDeleted = idx;  
        }
        if (h.slots[idx].state == SLOT_EMPTY) {
            int target = (firstDeleted != -1) ? firstDeleted : idx;
            if (h.slots[target].state == SLOT_DELETED) h.deletedCount--;
            h.slots[target].key = value;
            h.slots[target].state = SLOT_OCCUPIED;
            h.size++;
            return makeOk(value);
        }
    }

    if (firstDeleted != -1) {
        h.slots[firstDeleted].key = value;
        h.slots[firstDeleted].state = SLOT_OCCUPIED;
        h.deletedCount--;
        h.size++;
        return makeOk(value);
    }

    return makeError("ERROR: hash table is full");
}


CommandResult htFind(const HashTable& h, const string& value) {
    for (int i = 0; i < h.capacity; ++i) {
        int idx = probeIndex(h, value, i);
        if (h.slots[idx].state == SLOT_EMPTY) {
            return makeOk("FALSE");   
        }
        if (h.slots[idx].state == SLOT_OCCUPIED && h.slots[idx].key == value) {
            return makeOk("TRUE");
        }
    }
    return makeOk("FALSE");
}

CommandResult htRemove(HashTable& h, const string& value) {
    for (int i = 0; i < h.capacity; ++i) {
        int idx = probeIndex(h, value, i);
        if (h.slots[idx].state == SLOT_EMPTY) {
            return makeError("ERROR: value not found");
        }
        if (h.slots[idx].state == SLOT_OCCUPIED && h.slots[idx].key == value) {
            h.slots[idx].state = SLOT_DELETED;  
            h.size--;
            h.deletedCount++;
            return makeOk(value);
        }
    }
    return makeError("ERROR: value not found");
}

CommandResult htPrint(const HashTable& h) {
    ostringstream oss;
    oss << "{";
    bool first = true;
    for (int i = 0; i < h.capacity; ++i) {
        if (h.slots[i].state == SLOT_OCCUPIED) {
            if (!first) oss << ", ";
            oss << h.slots[i].key;
            first = false;
        }
    }
    oss << "}";
    return makeOk(oss.str());
}

CommandResult htStats(const HashTable& h) {
    ostringstream oss;
    int occupied = 0, deleted = 0, empty = 0;
    int maxRun = 0, curRun = 0;

    for (int i = 0; i < h.capacity; ++i) {
        if (h.slots[i].state == SLOT_OCCUPIED) { occupied++; curRun++; }
        else { 
            if (curRun > maxRun) maxRun = curRun; 
            curRun = 0;
            if (h.slots[i].state == SLOT_DELETED) deleted++;
            else empty++;
        }
    }
    if (curRun > maxRun) maxRun = curRun;

    oss << "mode=" << (h.mode == PROBE_LINEAR ? "linear" : "double")
        << " size=" << h.size
        << " occupied=" << occupied
        << " deleted=" << deleted
        << " empty=" << empty
        << " load_factor=" << (double)h.size / h.capacity
        << " max_cluster_run=" << maxRun;
    return makeOk(oss.str());
}

CommandResult htInsertWithValue(HashTable& h, const string& value, int intVal) {
    int firstDeleted = -1;
    for (int i = 0; i < h.capacity; ++i) {
        int idx = probeIndex(h, value, i);
        if (h.slots[idx].state == SLOT_OCCUPIED && h.slots[idx].key == value) {
            h.slots[idx].value = intVal; 
            return makeOk(value);
        }
        if (h.slots[idx].state == SLOT_DELETED && firstDeleted == -1) firstDeleted = idx;
        if (h.slots[idx].state == SLOT_EMPTY) {
            int target = (firstDeleted != -1) ? firstDeleted : idx;
            if (h.slots[target].state == SLOT_DELETED) h.deletedCount--;
            h.slots[target].key = value;
            h.slots[target].value = intVal;
            h.slots[target].state = SLOT_OCCUPIED;
            h.size++;
            return makeOk(value);
        }
    }
    return makeError("ERROR: hash table is full");
}

bool htFindWithValue(const HashTable& h, const string& value, int& outVal) {
    for (int i = 0; i < h.capacity; ++i) {
        int idx = probeIndex(h, value, i);
        if (h.slots[idx].state == SLOT_EMPTY) return false;
        if (h.slots[idx].state == SLOT_OCCUPIED && h.slots[idx].key == value) {
            outVal = h.slots[idx].value;
            return true;
        }
    }
    return false;
}

CommandResult htLongestUnique(const HashTable& baseHt, const string& s) {
    if (s.empty()) return makeOk("0 ()");

    HashTable ht;
    htInit(ht, "temp", baseHt.mode, 257); 

    int maxLen = 0;
    int maxStart = 0;
    int left = 0;

    for (int right = 0; right < (int)s.length(); ++right) {
        string ch(1, s[right]);
        int prevIdx = -1;

        if (htFindWithValue(ht, ch, prevIdx) && prevIdx >= left) {
            left = prevIdx + 1;
        }

        htInsertWithValue(ht, ch, right);

        int curLen = right - left + 1;
        if (curLen > maxLen) {
            maxLen = curLen;
            maxStart = left;
        }
    }

    string longestStr = s.substr(maxStart, maxLen);
    htClear(ht); 

    ostringstream oss;
    oss << maxLen << " (" << longestStr << ")";
    return makeOk(oss.str());
}