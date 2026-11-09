use crate::common::*;
use crate::command_parser::ParsedCommand;
use crate::containers::Containers;
use crate::hashtable::{ht_longest_unique, ProbeMode};
use crate::split::split_set;
use crate::subsets::print_all_subsets;

fn check_name(expected: &str, actual: &str) -> bool {
    !expected.is_empty() && expected == actual
}

pub fn execute_command(c: &mut Containers, pc: &ParsedCommand) -> CommandResult {
    let t = &pc.tokens;
    if t.is_empty() { return make_error("ERROR: empty command"); }
    let cmd = t[0].as_str();

    if cmd == "PRINT" {
        if t.len() < 2 { return make_error("ERROR: PRINT requires name"); }
        let name = &t[1];
        if name == &c.arr.name { return c.arr.print(); }
        if name == &c.slist.name { return c.slist.print(); }
        if name == &c.dlist.name { return c.dlist.print(); }
        if name == &c.stk.name { return c.stk.print(); }
        if name == &c.q.name { return c.q.print(); }
        if name == &c.tree.name { return c.tree.print(); }
        if name == &c.set.name { return c.set.print(); }
        return make_error("ERROR: unknown container name");
    }

    if cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT" {
        if t.len() != 3 { return make_error("ERROR: bad SET command args"); }
        if !check_name(&c.set.name, &t[1]) { return make_error("ERROR: name mismatch"); }
        return match cmd {
            "SETADD" => c.set.add(&t[2]),
            "SETDEL" => c.set.del(&t[2]),
            _ => c.set.at(&t[2]),
        };
    }

    if cmd == "SPLIT" {
        if t.len() != 3 { return make_error("ERROR: bad SPLIT args"); }
        if !check_name(&c.set.name, &t[1]) { return make_error("ERROR: name mismatch"); }
        let target: i32 = match t[2].parse() {
            Ok(n) => n,
            Err(_) => return make_error("ERROR: target must be integer"),
        };
        return split_set(&c.set, target);
    }

    if cmd == "BSTADD" || cmd == "BSTHEIGHT" || cmd == "BSTPRINT" {
        if t.len() < 2 { return make_error("ERROR: missing BST name"); }
        if !check_name(&c.bst.name, &t[1]) { return make_error("ERROR: name mismatch"); }
        if cmd == "BSTADD" {
            if t.len() != 3 { return make_error("ERROR: bad BSTADD args"); }
            let v: i32 = match t[2].parse() {
                Ok(n) => n,
                Err(_) => return make_error("ERROR: value must be integer"),
            };
            return c.bst.insert(v);
        }
        if cmd == "BSTHEIGHT" { return c.bst.height_result(); }
        if cmd == "BSTPRINT" { return c.bst.print(); }
    }

    if cmd == "HLONGEST" {
        if t.len() < 3 { return make_error("ERROR: bad HLONGEST args"); }
        if !check_name(&c.ht.name, &t[1]) { return make_error("ERROR: name mismatch"); }
        let mut s = t[2].clone();
        for extra in &t[3..] { s.push(' '); s.push_str(extra); }
        return ht_longest_unique(c.ht.mode, &s);
    }

    if cmd.starts_with('H') {
        if t.len() < 2 { return make_error("ERROR: missing hash table name"); }
        if cmd == "HLINIT" {
            if t.len() != 3 { return make_error("ERROR: bad HLINIT args"); }
            if c.ht.capacity > 0 { c.ht.clear(); }
            let mode = if t[2] == "linear" { ProbeMode::Linear } else { ProbeMode::Double };
            c.ht = crate::hashtable::HashTable::new(&t[1], mode, 10007);
            return make_ok(t[1].clone());
        }
        if !check_name(&c.ht.name, &t[1]) { return make_error("ERROR: name mismatch"); }
        if cmd == "HINSERT" {
            if t.len() != 3 { return make_error("ERROR: bad HINSERT args"); }
            return c.ht.insert(&t[2]);
        }
        if cmd == "HREMOVE" {
            if t.len() != 3 { return make_error("ERROR: bad HREMOVE args"); }
            return c.ht.remove(&t[2]);
        }
        if cmd == "HFIND" {
            if t.len() != 3 { return make_error("ERROR: bad HFIND args"); }
            return c.ht.find(&t[2]);
        }
        if cmd == "HPRINT" { return c.ht.print(); }
        if cmd == "HSTATS" { return c.ht.stats(); }
        return make_error("ERROR: unknown hash command");
    }

    if cmd == "LRUINIT" {
        if t.len() != 2 { return make_error("ERROR: bad LRUINIT args"); }
        let cap: i32 = match t[1].parse() {
            Ok(n) => n,
            Err(_) => return make_error("ERROR: capacity must be integer"),
        };
        if cap <= 0 { return make_error("ERROR: capacity must be positive"); }
        c.lru.clear();
        c.lru.init(cap);
        return make_ok(cap.to_string());
    }
    if cmd == "LRUSET" {
        if t.len() != 3 { return make_error("ERROR: bad LRUSET args"); }
        if !c.lru.initialized { return make_error("ERROR: LRU not initialized (call LRUINIT first)"); }
        return c.lru.set(&t[1], &t[2]);
    }
    if cmd == "LRUGET" {
        if t.len() != 2 { return make_error("ERROR: bad LRUGET args"); }
        if !c.lru.initialized { return make_error("ERROR: LRU not initialized (call LRUINIT first)"); }
        return c.lru.get(&t[1]);
    }
    if cmd == "LRUPRINT" {
        if !c.lru.initialized { return make_error("ERROR: LRU not initialized (call LRUINIT first)"); }
        return c.lru.print();
    }

    match pc.container_type {
        ContainerType::Array => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.arr.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "MSUB" {
                if t.len() != 2 { return make_error("ERROR: bad MSUB args"); }
                return print_all_subsets(&c.arr);
            }
            if cmd == "MPUSH" {
                if t.len() == 3 { return c.arr.push(&t[2], -1); }
                if t.len() == 4 {
                    let idx: i32 = match t[3].parse() {
                        Ok(n) => n,
                        Err(_) => return make_error("ERROR: bad index"),
                    };
                    return c.arr.push(&t[2], idx);
                }
                return make_error("ERROR: bad MPUSH args");
            }
            if cmd == "MDEL" {
                if t.len() != 3 { return make_error("ERROR: bad MDEL args"); }
                let idx: i32 = match t[2].parse() {
                    Ok(n) => n,
                    Err(_) => return make_error("ERROR: bad index"),
                };
                return c.arr.del(idx);
            }
            if cmd == "MGET" {
                if t.len() != 3 { return make_error("ERROR: bad MGET args"); }
                let idx: i32 = match t[2].parse() {
                    Ok(n) => n,
                    Err(_) => return make_error("ERROR: bad index"),
                };
                return c.arr.get(idx);
            }
            if cmd == "MSET" {
                if t.len() != 4 { return make_error("ERROR: bad MSET args"); }
                let idx: i32 = match t[2].parse() {
                    Ok(n) => n,
                    Err(_) => return make_error("ERROR: bad index"),
                };
                return c.arr.set(idx, &t[3]);
            }
            if cmd == "MLEN" { return c.arr.len(); }
            make_error("ERROR: unknown array command")
        }
        ContainerType::Singly => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.slist.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "FPUSH" {
                if t.len() == 4 {
                    return match t[3].as_str() {
                        "head" => c.slist.push_head(&t[2]),
                        "tail" => c.slist.push_tail(&t[2]),
                        _ => make_error("ERROR: bad FPUSH mode"),
                    };
                }
                if t.len() == 5 {
                    return match t[3].as_str() {
                        "after" => c.slist.push_after(&t[2], &t[4]),
                        "before" => c.slist.push_before(&t[2], &t[4]),
                        _ => make_error("ERROR: bad FPUSH mode"),
                    };
                }
                return make_error("ERROR: bad FPUSH args");
            }
            if cmd == "FDEL" {
                if t.len() == 3 {
                    return match t[2].as_str() {
                        "head" => c.slist.del_head(),
                        "tail" => c.slist.del_tail(),
                        _ => make_error("ERROR: bad FDEL mode"),
                    };
                }
                if t.len() == 4 {
                    if t[2] == "after" { return c.slist.del_near(&t[3], true); }
                    if t[2] == "before" { return c.slist.del_near(&t[3], false); }
                    if t[3] == "value" { return c.slist.del_value(&t[2]); }
                    return make_error("ERROR: bad FDEL mode");
                }
                return make_error("ERROR: bad FDEL args");
            }
            if cmd == "FGET" { return c.slist.get(); }
            if cmd == "FFIND" {
                if t.len() != 3 { return make_error("ERROR: bad FFIND args"); }
                return c.slist.find(&t[2]);
            }
            make_error("ERROR: unknown singly command")
        }
        ContainerType::Doubly => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.dlist.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "LPUSH" {
                if t.len() == 4 {
                    return match t[3].as_str() {
                        "head" => c.dlist.push_head(&t[2]),
                        "tail" => c.dlist.push_tail(&t[2]),
                        _ => make_error("ERROR: bad LPUSH mode"),
                    };
                }
                if t.len() == 5 {
                    return match t[3].as_str() {
                        "after" => c.dlist.push_after(&t[2], &t[4]),
                        "before" => c.dlist.push_before(&t[2], &t[4]),
                        _ => make_error("ERROR: bad LPUSH mode"),
                    };
                }
                return make_error("ERROR: bad LPUSH args");
            }
            if cmd == "LDEL" {
                if t.len() == 3 {
                    return match t[2].as_str() {
                        "head" => c.dlist.del_head(),
                        "tail" => c.dlist.del_tail(),
                        _ => make_error("ERROR: bad LDEL mode"),
                    };
                }
                if t.len() == 4 {
                    if t[2] == "after" { return c.dlist.del_near(&t[3], true); }
                    if t[2] == "before" { return c.dlist.del_near(&t[3], false); }
                    if t[3] == "value" { return c.dlist.del_value(&t[2]); }
                    return make_error("ERROR: bad LDEL mode");
                }
                return make_error("ERROR: bad LDEL args");
            }
            if cmd == "LGET" { return c.dlist.get(); }
            if cmd == "LFIND" {
                if t.len() != 3 { return make_error("ERROR: bad LFIND args"); }
                return c.dlist.find(&t[2]);
            }
            make_error("ERROR: unknown doubly command")
        }
        ContainerType::Stack => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.stk.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "SPUSH" {
                if t.len() != 3 { return make_error("ERROR: bad SPUSH args"); }
                return c.stk.push(&t[2]);
            }
            if cmd == "SPOP" { return c.stk.pop(); }
            make_error("ERROR: unknown stack command")
        }
        ContainerType::Queue => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.q.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "QPUSH" {
                if t.len() != 3 { return make_error("ERROR: bad QPUSH args"); }
                return c.q.push(&t[2]);
            }
            if cmd == "QPOP" { return c.q.pop(); }
            make_error("ERROR: unknown queue command")
        }
        ContainerType::Tree => {
            if t.len() < 2 { return make_error("ERROR: missing name"); }
            if !check_name(&c.tree.name, &t[1]) { return make_error("ERROR: name mismatch"); }
            if cmd == "TINSERT" {
                if t.len() != 3 { return make_error("ERROR: bad TINSERT args"); }
                let v: i32 = match t[2].parse() {
                    Ok(n) => n,
                    Err(_) => return make_error("ERROR: value must be integer"),
                };
                return c.tree.insert(v);
            }
            if cmd == "TGET" {
                if t.len() != 3 { return make_error("ERROR: bad TGET args"); }
                let v: i32 = match t[2].parse() {
                    Ok(n) => n,
                    Err(_) => return make_error("ERROR: value must be integer"),
                };
                return c.tree.get(v);
            }
            if cmd == "TCHECK" { return c.tree.check(); }
            make_error("ERROR: unknown tree command")
        }
        ContainerType::Unknown => make_error("ERROR: unknown command"),
    }
}