#ifndef STORAGE_H
#define STORAGE_H

#include "common.h"

struct StorageMeta {
    string arrayName;
    string singlyName;
    string doublyName;
    string stackName;
    string queueName;
    string treeName;
    string setName;
    string bstName; 
    string htName;
};

StorageMeta loadMeta(const string& filename);

string extractValue(const string& line, const string& key);
vector<string> splitCSV(const string& csv, bool keepEmpty = false);

#endif
