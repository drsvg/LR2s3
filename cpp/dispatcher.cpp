#include "dispatcher.h"
#include "tasks_check.h"
#include "subsets.h"

static bool checkName(const string& expected, const string& actual) {
    return !expected.empty() && expected == actual;
}

CommandResult executeCommand(Containers& c, const ParsedCommand& pc) {
    const vector<string>& t = pc.tokens;
    if (t.empty()) return makeError("ERROR: empty command");

    const string& cmd = t[0];

    if (cmd == "PRINT") {
        if (t.size() < 2) return makeError("ERROR: PRINT requires name");
        const string& name = t[1];
        if (name == c.arr.name) return arrayPrint(c.arr);
        if (name == c.slist.name) return singlyPrint(c.slist);
        if (name == c.dlist.name) return doublyPrint(c.dlist);
        if (name == c.stk.name) return stackPrint(c.stk);
        if (name == c.q.name) return queuePrint(c.q);
        if (name == c.tree.name) return treePrint(c.tree);
        if (name == c.set.name)  return setPrint(c.set);
        return makeError("ERROR: unknown container name");
    }

    if (cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT") {
        if (t.size() != 3) return makeError("ERROR: bad SET command args");
        if (!checkName(c.set.name, t[1])) return makeError("ERROR: name mismatch");
        if (cmd == "SETADD") return setAdd(c.set, t[2]);
        if (cmd == "SETDEL") return setDel(c.set, t[2]);
        return setAt(c.set, t[2]);
    }

    if (cmd == "SPLIT") {
        if (t.size() != 3) return makeError("ERROR: bad SPLIT args");
        if (!checkName(c.set.name, t[1])) return makeError("ERROR: name mismatch");
        int target;
        try {
            target = stoi(t[2]);
        } catch (...) {
            return makeError("ERROR: target must be integer");
        }
        return splitSet(c.set, target);
    }

    if (cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT") {
        if (t.size() != 3) return makeError("ERROR: bad SET command args");
        if (!checkName(c.set.name, t[1])) return makeError("ERROR: name mismatch");
        if (cmd == "SETADD") return setAdd(c.set, t[2]);
        if (cmd == "SETDEL") return setDel(c.set, t[2]);
        return setAt(c.set, t[2]);  
    }

    if (cmd == "BSTADD" || cmd == "BSTHEIGHT" || cmd == "BSTPRINT") {
        if (t.size() < 2) return makeError("ERROR: missing BST name");
        if (!checkName(c.bst.name, t[1])) return makeError("ERROR: name mismatch");
        
        if (cmd == "BSTADD") {
            if (t.size() != 3) return makeError("ERROR: bad BSTADD args");
            int v;
            try {
                v = stoi(t[2]);
            } catch (...) {
                return makeError("ERROR: value must be integer");
            }
            return bstInsert(c.bst, v);
        }
        if (cmd == "BSTHEIGHT") return bstHeight(c.bst);
        if (cmd == "BSTPRINT") return bstPrint(c.bst);
    }

    if (cmd == "HLONGEST") {
        if (t.size() < 3) return makeError("ERROR: bad HLONGEST args (need: HLONGEST <name> <string>)");
        if (!checkName(c.ht.name, t[1])) return makeError("ERROR: name mismatch");
        
        string str = t[2];
        for (size_t i = 3; i < t.size(); ++i) {
            str += " " + t[i];
        }
        return htLongestUnique(c.ht, str);
    }

    if (cmd.rfind("H", 0) == 0) {   
        if (t.size() < 2) return makeError("ERROR: missing hash table name");
        if (cmd == "HLINIT") {
            if (t.size() != 3) return makeError("ERROR: bad HLINIT args");
            
            if (c.ht.slots != nullptr) {
                htClear(c.ht);
            }
            ProbeMode m = (t[2] == "linear") ? PROBE_LINEAR : PROBE_DOUBLE;
            htInit(c.ht, t[1], m);
            return makeOk(t[1]);
        }
        if (!checkName(c.ht.name, t[1])) return makeError("ERROR: name mismatch");
        if (cmd == "HINSERT") {
            if (t.size() != 3) return makeError("ERROR: bad HINSERT args");
            return htInsert(c.ht, t[2]);
        }
        if (cmd == "HREMOVE") {
            if (t.size() != 3) return makeError("ERROR: bad HREMOVE args");
            return htRemove(c.ht, t[2]);
        }
        if (cmd == "HFIND") {
            if (t.size() != 3) return makeError("ERROR: bad HFIND args");
            return htFind(c.ht, t[2]);
        }
        if (cmd == "HPRINT") return htPrint(c.ht);
        if (cmd == "HSTATS") return htStats(c.ht);
        return makeError("ERROR: unknown hash command");
    }

    if (cmd == "LRUINIT") {
        if (t.size() != 2) return makeError("ERROR: bad LRUINIT args (need: LRUINIT <cap>)");
        int cap;
        try { cap = stoi(t[1]); }
        catch (...) { return makeError("ERROR: capacity must be integer"); }
        if (cap <= 0) return makeError("ERROR: capacity must be positive");
        if (c.lru.buckets) lruClear(c.lru); 
        lruInit(c.lru, cap);
        return makeOk(to_string(cap));
    }
    if (cmd == "LRUSET") {
        if (t.size() != 3) return makeError("ERROR: bad LRUSET args (need: LRUSET <key> <value>)");
        if (!c.lru.buckets) return makeError("ERROR: LRU not initialized (call LRUINIT first)");
        return lruSet(c.lru, t[1], t[2]);
    }
    if (cmd == "LRUGET") {
        if (t.size() != 2) return makeError("ERROR: bad LRUGET args");
        if (!c.lru.buckets) return makeError("ERROR: LRU not initialized (call LRUINIT first)");
        return lruGet(c.lru, t[1]);
    }
    if (cmd == "LRUPRINT") {
        if (!c.lru.buckets) return makeError("ERROR: LRU not initialized (call LRUINIT first)");
        return lruPrint(c.lru);
    }

    switch (pc.type) {
        case CT_ARRAY: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            const string& name = t[1];
            if (!checkName(c.arr.name, name)) return makeError("ERROR: name mismatch");
                if (cmd == "MSUB") {
                if (t.size() != 2) return makeError("ERROR: bad MSUB args");
                if (!checkName(c.arr.name, t[1])) return makeError("ERROR: name mismatch");
                return printAllSubsets(c.arr);
            }
            if (cmd == "MPUSH") {
                if (t.size() == 3) return arrayPush(c.arr, t[2]);
                if (t.size() == 4) return arrayPush(c.arr, t[2], stoi(t[3]));
                return makeError("ERROR: bad MPUSH args");
            }
            if (cmd == "MDEL") {
                if (t.size() != 3) return makeError("ERROR: bad MDEL args");
                return arrayDel(c.arr, stoi(t[2]));
            }
            if (cmd == "MGET") {
                if (t.size() != 3) return makeError("ERROR: bad MGET args");
                return arrayGet(c.arr, stoi(t[2]));
            }
            if (cmd == "MSET") {
                if (t.size() != 4) return makeError("ERROR: bad MSET args");
                return arraySet(c.arr, stoi(t[2]), t[3]);
            }
            if (cmd == "MLEN") {
                return arrayLen(c.arr);
            }
            return makeError("ERROR: unknown array command");
        }
                case CT_SINGLY: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            if (!checkName(c.slist.name, t[1])) return makeError("ERROR: name mismatch");
            
            if (cmd == "FPUSH") {
                if (t.size() == 4) {
                    if (t[3] == "head") return singlyPushHead(c.slist, t[2]);
                    if (t[3] == "tail") return singlyPushTail(c.slist, t[2]);
                    return makeError("ERROR: bad FPUSH mode");
                }
                if (t.size() == 5) {
                    if (t[3] == "after") return singlyPushAfter(c.slist, t[2], t[4]);
                    if (t[3] == "before") return singlyPushBefore(c.slist, t[2], t[4]);
                    return makeError("ERROR: bad FPUSH mode");
                }
                return makeError("ERROR: bad FPUSH args");
            }
            
            if (cmd == "FDEL") {
                if (t.size() == 3) {
                    if (t[2] == "head") return singlyDelHead(c.slist);
                    if (t[2] == "tail") return singlyDelTail(c.slist);
                    return makeError("ERROR: bad FDEL mode");
                }
                if (t.size() == 4) {
                    if (t[2] == "after") return singlyDelNear(c.slist, t[3], true);
                    if (t[2] == "before") return singlyDelNear(c.slist, t[3], false);
                    if (t[3] == "value") return singlyDelValue(c.slist, t[2]);
                    return makeError("ERROR: bad FDEL mode");
                }
                return makeError("ERROR: bad FDEL args");
            }
            
            if (cmd == "FGET") return singlyGet(c.slist);
            if (cmd == "FFIND") {
                if (t.size() != 3) return makeError("ERROR: bad FFIND args");
                return singlyFind(c.slist, t[2]);
            }
            return makeError("ERROR: unknown singly command");
        }

        case CT_DOUBLY: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            if (!checkName(c.dlist.name, t[1])) return makeError("ERROR: name mismatch");

            if (cmd == "LPUSH") {
                if (t.size() == 4) {
                    if (t[3] == "head") return doublyPushHead(c.dlist, t[2]);
                    if (t[3] == "tail") return doublyPushTail(c.dlist, t[2]);
                    return makeError("ERROR: bad LPUSH mode");
                }
                if (t.size() == 5) {
                    if (t[3] == "after") return doublyPushAfter(c.dlist, t[2], t[4]);
                    if (t[3] == "before") return doublyPushBefore(c.dlist, t[2], t[4]);
                    return makeError("ERROR: bad LPUSH mode");
                }
                return makeError("ERROR: bad LPUSH args");
            }

            if (cmd == "LDEL") {
                if (t.size() == 3) {
                    if (t[2] == "head") return doublyDelHead(c.dlist);
                    if (t[2] == "tail") return doublyDelTail(c.dlist);
                    return makeError("ERROR: bad LDEL mode");
                }
                if (t.size() == 4) {
                    if (t[2] == "after") return doublyDelNear(c.dlist, t[3], true);
                    if (t[2] == "before") return doublyDelNear(c.dlist, t[3], false);
                    if (t[3] == "value") return doublyDelValue(c.dlist, t[2]);
                    return makeError("ERROR: bad LDEL mode");
                }
                return makeError("ERROR: bad LDEL args");
            }

            if (cmd == "LGET") return doublyGet(c.dlist);
            if (cmd == "LFIND") {
                if (t.size() != 3) return makeError("ERROR: bad LFIND args");
                return doublyFind(c.dlist, t[2]);
            }
            return makeError("ERROR: unknown doubly command");
        }
        
        case CT_STACK: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            const string& name = t[1];
            if (!checkName(c.stk.name, name)) return makeError("ERROR: name mismatch");
            if (cmd == "SPUSH") {
                if (t.size() != 3) return makeError("ERROR: bad SPUSH args");
                return stackPush(c.stk, t[2]);
            }
            if (cmd == "SPOP") return stackPop(c.stk);
            return makeError("ERROR: unknown stack command");
        }
        case CT_QUEUE: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            const string& name = t[1];
            if (!checkName(c.q.name, name)) return makeError("ERROR: name mismatch");
            if (cmd == "QPUSH") {
                if (t.size() != 3) return makeError("ERROR: bad QPUSH args");
                return queuePush(c.q, t[2]);
            }
            if (cmd == "QPOP") return queuePop(c.q);
            return makeError("ERROR: unknown queue command");
        }
        case CT_TREE: {
            if (t.size() < 2) return makeError("ERROR: missing name");
            const string& name = t[1];
            if (!checkName(c.tree.name, name)) return makeError("ERROR: name mismatch");
            if (cmd == "TINSERT") {
                if (t.size() != 3) return makeError("ERROR: bad TINSERT args");
                return treeInsert(c.tree, stoi(t[2]));
            }
            if (cmd == "TGET") {
                if (t.size() != 3) return makeError("ERROR: bad TGET args");
                return treeGet(c.tree, stoi(t[2]));
            }
            if (cmd == "TCHECK") return treeCheck(c.tree);
            return makeError("ERROR: unknown tree command");
        }
        default:
            return makeError("ERROR: unknown command");
    }
}
