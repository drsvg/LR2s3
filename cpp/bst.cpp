#include "bst.h"
#include <algorithm>

void bstInit(BST& t, const string& name) {
    t.name = name;
    t.root = nullptr;
    t.size = 0;
}

static void clearNode(BSTNode* n) {
    if (!n) return;
    clearNode(n->left);
    clearNode(n->right);
    delete n;
}

void bstClear(BST& t) {
    clearNode(t.root);
    t.root = nullptr;
    t.size = 0;
}

static bool insertNode(BSTNode*& node, int value) {
    if (!node) {
        node = new BSTNode{value, nullptr, nullptr};
        return true;
    }
    if (value == node->value) return false;      
    if (value < node->value) return insertNode(node->left, value);
    return insertNode(node->right, value);
}

static void preorderNode(BSTNode* n, ostream& out) {
    if (!n) return;
    out << n->value << ",";
    preorderNode(n->left, out);
    preorderNode(n->right, out);
}
void bstPreorder(const BST& t, ostream& out) { preorderNode(t.root, out); }

CommandResult bstInsert(BST& t, int value) {
    if (insertNode(t.root, value)) {
        t.size++;
        return makeOk(to_string(value));
    }
    return makeOk(to_string(value)); 
}

static int height(BSTNode* n) {
    if (!n) return 0;
    return 1 + max(height(n->left), height(n->right));
}

CommandResult bstHeight(const BST& t) {
    return makeOk(to_string(height(t.root)));
}

static void inorder(BSTNode* n, ostringstream& oss, bool& first) {
    if (!n) return;
    inorder(n->left, oss, first);
    if (!first) oss << " ";
    oss << n->value;
    first = false;
    inorder(n->right, oss, first);
}

CommandResult bstPrint(const BST& t) {
    ostringstream oss;
    oss << "[";
    bool first = true;
    inorder(t.root, oss, first);
    oss << "]";
    return makeOk(oss.str());
}