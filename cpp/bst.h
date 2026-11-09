#ifndef BST_H
#define BST_H

#include "common.h"

struct BSTNode {
    int value;
    BSTNode* left;
    BSTNode* right;
};

struct BST {
    string name;
    BSTNode* root;
    int size;
};

void bstInit(BST& t, const string& name);
void bstClear(BST& t);
void bstPreorder(const BST& t, ostream& out);

CommandResult bstInsert(BST& t, int value);

CommandResult bstHeight(const BST& t);

CommandResult bstPrint(const BST& t);

#endif