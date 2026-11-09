#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "common.h"

enum ProbeMode { PROBE_LINEAR, PROBE_DOUBLE };

enum SlotState { SLOT_EMPTY, SLOT_OCCUPIED, SLOT_DELETED };

struct Slot {
    string key;
    int value;
    SlotState state;
};

struct HashTable {
    string name;
    Slot* slots;          
    int capacity;    
    int size;        
    int deletedCount;      
    ProbeMode mode;       
};

void htInit(HashTable& h, const string& name, ProbeMode mode, int capacity = 10007);
void htClear(HashTable& h);

CommandResult htInsert(HashTable& h, const string& value);
CommandResult htRemove(HashTable& h, const string& value);
CommandResult htFind(const HashTable& h, const string& value);
CommandResult htPrint(const HashTable& h);

CommandResult htStats(const HashTable& h);

CommandResult htLongestUnique(const HashTable& h, const string& s);

#endif