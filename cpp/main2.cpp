#include "lrucache.h"
#include <iostream>
using namespace std;

int main() {
    LRUCache cache;
    lruInit(cache, 2);

    // Эмуляция: SET 1 2, SET 2 3, SET 1 5, SET 4 5, SET 6 7, GET 4, SET 1 2, GET 3
    lruSet(cache, "1", "2");
    lruSet(cache, "2", "3");
    lruSet(cache, "1", "5");
    lruSet(cache, "4", "5");
    lruSet(cache, "6", "7");
    
    cout << "GET 4: " << lruGet(cache, "4").output << endl; // Ожидается 5
    
    lruSet(cache, "1", "2");
    
    cout << "GET 3: " << lruGet(cache, "3").output << endl; // Ожидается -1

    lruClear(cache);
    return 0;
}