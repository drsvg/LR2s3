#include "common.h"
#include "command_parser.h"
#include "dispatcher.h"
#include "storage.h"
#include <cstring>
#include <queue>

static void loadContainers(Containers& c, const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        if (line.find("|array") != string::npos) {
            string csv = extractValue(line, c.arr.name + "|array");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) arrayPush(c.arr, v);
        } 
                else if (line.find("|flist") != string::npos) {
            string csv = extractValue(line, c.slist.name + "|flist");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) singlyPushTail(c.slist, v);
        } 
        else if (line.find("|dlist") != string::npos) {
            string csv = extractValue(line, c.dlist.name + "|dlist");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) doublyPushTail(c.dlist, v); 
        } 
        else if (line.find("|stack") != string::npos) {
            string csv = extractValue(line, c.stk.name + "|stack");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) stackPush(c.stk, v);
        } 
        else if (line.find("|queue") != string::npos) {
            string csv = extractValue(line, c.q.name + "|queue");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) queuePush(c.q, v);
        } 
        else if (line.find("|tree") != string::npos) {
            string csv = extractValue(line, c.tree.name + "|tree");
            vector<string> vals = splitCSV(csv, true); 
            for (const string& v : vals) {
                if (v.empty()) {
                    treeInsertEmpty(c.tree);
                } else {
                    treeInsert(c.tree, stoi(v));
                }
            }
        }
        else if (line.find("|set") != string::npos) {
            string csv = extractValue(line, c.set.name + "|set");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) setAdd(c.set, v);
        }
        else if (line.find("|bst") != string::npos) {
            string csv = extractValue(line, c.bst.name + "|bst");
            vector<string> vals = splitCSV(csv);
            for (const string& v : vals) bstInsert(c.bst, stoi(v));
        }
        else if (line.find("|ht") != string::npos) {
            string csv = extractValue(line, c.ht.name + "|ht");
            size_t colon = csv.find(':');
            
            if (colon != string::npos) {
                string modeStr = csv.substr(0, colon);
                string data = csv.substr(colon + 1);
                
                htClear(c.ht);
                ProbeMode mode = (modeStr == "linear") ? PROBE_LINEAR : PROBE_DOUBLE;
                htInit(c.ht, c.ht.name, mode); 
                
                vector<string> vals = splitCSV(data);
                for (const string& v : vals) htInsert(c.ht, v);
            }
        }
    }
}

static void saveContainers(const Containers& c, const string& filename, const StorageMeta& meta) {
    ofstream out(filename);
    if (!out.is_open()) return;

    out << "{\n";

    out << "    \"" << meta.arrayName << "|array\": \"";
    for (int i = 0; i < c.arr.size; ++i) {
        out << c.arr.data[i] << ",";
    }
    out << "\",\n";

    out << "    \"" << meta.singlyName << "|flist\": \"";
    for (SNode* n = c.slist.head; n; n = n->next) {
        out << n->value << ",";
    }
    out << "\",\n";

    out << "    \"" << meta.doublyName << "|dlist\": \"";
    for (DNode* n = c.dlist.head; n; n = n->next) {
        out << n->value << ",";
    }
    out << "\",\n";

    out << "    \"" << meta.stackName << "|stack\": \"";
    vector<string> stkItems;
    for (StackNode* n = c.stk.top; n; n = n->next) {
        stkItems.push_back(n->value);
    }
    for (int i = (int)stkItems.size() - 1; i >= 0; --i) {
        out << stkItems[i] << ",";
    }
    out << "\",\n";

    out << "    \"" << meta.queueName << "|queue\": \"";
    for (QueueNode* n = c.q.front; n; n = n->next) {
        out << n->value << ",";
    }
    out << "\",\n";

    out << "    \"" << meta.treeName << "|tree\": \"";
    if (c.tree.root) {
        queue<TNode*> qq;
        qq.push(c.tree.root);
        while (!qq.empty()) {
            TNode* cur = qq.front(); 
            qq.pop();
            out << cur->value << ",";
            if (cur->left) qq.push(cur->left);
            if (cur->right) qq.push(cur->right);
        }
    }
    out << "\",\n";  

    out << "    \"" << meta.setName << "|set\": \"";
    for (int i = 0; i < SET_BUCKETS; ++i) {
        for (SetNode* n = c.set.buckets[i]; n; n = n->next) {
            out << n->value << ",";
        }
    }
    out << "\",\n";  

    out << "    \"" << meta.bstName << "|bst\": \"";
    auto preorder = [&](auto&& self, BSTNode* n) -> void {
        if (!n) return;
        out << n->value << ",";
        self(self, n->left);
        self(self, n->right);
    };
    preorder(preorder, c.bst.root);
    
    out << "\",\n";  

    out << "    \"" << meta.htName << "|ht\": \"";
    if (c.ht.slots) {
        out << (c.ht.mode == PROBE_LINEAR ? "linear" : "double") << ":";
        for (int i = 0; i < c.ht.capacity; ++i) {
            if (c.ht.slots[i].state == SLOT_OCCUPIED) {
                out << c.ht.slots[i].key << ",";
            }
        }
    }
    out << "\"\n}";
}

static bool isMutating(const string& cmd) {
    return cmd == "MPUSH" || cmd == "MDEL" || cmd == "MSET" ||
            cmd == "FPUSH" || cmd == "FDEL" ||
            cmd == "LPUSH" || cmd == "LDEL" ||
            cmd == "SPUSH" || cmd == "SPOP" ||
            cmd == "QPUSH" || cmd == "QPOP" ||
            cmd == "TINSERT" ||
            cmd == "SETADD" || cmd == "SETDEL" ||
            cmd == "BSTADD" || cmd == "HLINIT" || 
            cmd == "HINSERT" || cmd == "HREMOVE";
}

int main(int argc, char* argv[]) {
    string filename;
    string query;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[++i];
        }
    }

    if (filename.empty() || query.empty()) {
        cout << "ERROR: usage: ./dbms --file <file> --query '<command>'" << endl;
        return 1;
    }

    StorageMeta meta = loadMeta(filename);

    if (meta.arrayName.empty()) meta.arrayName = "myarray";
    if (meta.singlyName.empty()) meta.singlyName = "mylist";
    if (meta.doublyName.empty()) meta.doublyName = "mydlist";
    if (meta.stackName.empty()) meta.stackName = "mystack";
    if (meta.queueName.empty()) meta.queueName = "myqueue";
    if (meta.treeName.empty()) meta.treeName = "mytree";
    if (meta.setName.empty()) meta.setName = "myset";
    if (meta.bstName.empty()) meta.bstName = "mybst";
    if (meta.htName.empty()) meta.htName = "myht";

    Containers c;
    arrayInit(c.arr, meta.arrayName);
    singlyInit(c.slist, meta.singlyName);
    doublyInit(c.dlist, meta.doublyName);
    stackInit(c.stk, meta.stackName);
    queueInit(c.q, meta.queueName);
    treeInit(c.tree, meta.treeName);
    setInit(c.set, meta.setName);
    bstInit(c.bst, meta.bstName);

    c.lru.buckets = nullptr;  
    c.lru.size = 0;
    c.lru.capacity = 0;

    c.ht.slots = nullptr; 
    htInit(c.ht, meta.htName, PROBE_LINEAR);

    loadContainers(c, filename);

    ParsedCommand pc = parseCommand(query);
    CommandResult res = executeCommand(c, pc);

    if (res.ok && isMutating(pc.command)) {
        saveContainers(c, filename, meta);
    }

    cout << res.output << endl;

    arrayClear(c.arr);
    singlyClear(c.slist);
    doublyClear(c.dlist);
    stackClear(c.stk);
    queueClear(c.q);
    treeClear(c.tree);
    setClear(c.set); 
    htClear(c.ht);
    lruClear(c.lru);

    return res.ok ? 0 : 1;
}
