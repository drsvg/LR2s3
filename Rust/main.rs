use dbms::command_parser::parse_command;
use dbms::common::*;
use dbms::containers::Containers;
use dbms::dispatcher::execute_command;
use dbms::hashtable::{HashTable, ProbeMode, SlotState};
use dbms::storage::{extract_value, load_meta, split_csv, StorageMeta};
use std::fs::File;
use std::io::{BufRead, BufReader, Write};
use std::process;

fn load_containers(c: &mut Containers, filename: &str) {
    let file = match File::open(filename) { Ok(f) => f, Err(_) => return };
    for line in BufReader::new(file).lines().flatten() {
        if line.contains("|array") {
            let csv = extract_value(&line, &format!("{}|array", c.arr.name));
            for v in split_csv(&csv, false) { c.arr.push(&v, -1); }
        } else if line.contains("|flist") {
            let csv = extract_value(&line, &format!("{}|flist", c.slist.name));
            for v in split_csv(&csv, false) { c.slist.push_tail(&v); }
        } else if line.contains("|dlist") {
            let csv = extract_value(&line, &format!("{}|dlist", c.dlist.name));
            for v in split_csv(&csv, false) { c.dlist.push_tail(&v); }
        } else if line.contains("|stack") {
            let csv = extract_value(&line, &format!("{}|stack", c.stk.name));
            for v in split_csv(&csv, false) { c.stk.push(&v); }
        } else if line.contains("|queue") {
            let csv = extract_value(&line, &format!("{}|queue", c.q.name));
            for v in split_csv(&csv, false) { c.q.push(&v); }
        } else if line.contains("|tree") {
            let csv = extract_value(&line, &format!("{}|tree", c.tree.name));
            for v in split_csv(&csv, true) {
                if v.is_empty() { c.tree.insert_empty(); }
                else if let Ok(n) = v.parse::<i32>() { c.tree.insert(n); }
            }
        } else if line.contains("|set") {
            let csv = extract_value(&line, &format!("{}|set", c.set.name));
            for v in split_csv(&csv, false) { c.set.add(&v); }
        } else if line.contains("|bst") {
            let csv = extract_value(&line, &format!("{}|bst", c.bst.name));
            for v in split_csv(&csv, false) {
                if let Ok(n) = v.parse::<i32>() { c.bst.insert(n); }
            }
        } else if line.contains("|ht") {
            let csv = extract_value(&line, &format!("{}|ht", c.ht.name));
            if let Some(colon) = csv.find(':') {
                let mode_str = &csv[..colon];
                let data = &csv[colon + 1..];
                c.ht.clear();
                let mode = if mode_str == "linear" { ProbeMode::Linear } else { ProbeMode::Double };
                c.ht = HashTable::new(&c.ht.name.clone(), mode, 10007);
                for v in split_csv(data, false) { c.ht.insert(&v); }
            }
        }
    }
}

fn save_containers(c: &Containers, filename: &str, meta: &StorageMeta) {
    let mut out = match File::create(filename) { Ok(f) => f, Err(_) => return };
    let _ = writeln!(out, "{{");

    let _ = write!(out, "    \"{}|array\": \"", meta.array_name);
    for v in &c.arr.data { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|flist\": \"", meta.singly_name);
    for v in &c.slist.items { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|dlist\": \"", meta.doubly_name);
    for v in &c.dlist.items { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|stack\": \"", meta.stack_name);
    for v in &c.stk.items { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|queue\": \"", meta.queue_name);
    for v in &c.q.items { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|tree\": \"", meta.tree_name);
    if let Some(root) = &c.tree.root {
        let mut queue = std::collections::VecDeque::new();
        queue.push_back(root.as_ref());
        while let Some(n) = queue.pop_front() {
            let _ = write!(out, "{},", n.value);
            if let Some(l) = &n.left { queue.push_back(l); }
            if let Some(r) = &n.right { queue.push_back(r); }
        }
    }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|set\": \"", meta.set_name);
    let mut set_items: Vec<&String> = c.set.items.iter().collect();
    set_items.sort();
    for v in set_items { let _ = write!(out, "{},", v); }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|bst\": \"", meta.bst_name);
    {
        fn preorder(n: &Option<Box<dbms::bst::BSTNode>>, out: &mut String) {
            if let Some(node) = n {
                out.push_str(&node.value.to_string());
                out.push(',');
                preorder(&node.left, out);
                preorder(&node.right, out);
            }
        }
        let mut s = String::new();
        preorder(&c.bst.root, &mut s);
        let _ = write!(out, "{}", s);
    }
    let _ = writeln!(out, "\",");

    let _ = write!(out, "    \"{}|ht\": \"", meta.ht_name);
    if c.ht.capacity > 0 {
        let mode = if c.ht.mode == ProbeMode::Linear { "linear" } else { "double" };
        let _ = write!(out, "{}:", mode);
        for s in &c.ht.slots {
            if s.state == SlotState::Occupied {
                let _ = write!(out, "{},", s.key);
            }
        }
    }
    let _ = writeln!(out, "\"\n}}");
}

fn is_mutating(cmd: &str) -> bool {
    matches!(cmd,
        "MPUSH" | "MDEL" | "MSET" |
        "FPUSH" | "FDEL" |
        "LPUSH" | "LDEL" |
        "SPUSH" | "SPOP" |
        "QPUSH" | "QPOP" |
        "TINSERT" |
        "SETADD" | "SETDEL" |
        "BSTADD" | "HLINIT" |
        "HINSERT" | "HREMOVE")
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut filename = String::new();
    let mut query = String::new();
    let mut i = 1;
    while i < args.len() {
        if args[i] == "--file" && i + 1 < args.len() {
            filename = args[i + 1].clone();
            i += 2;
        } else if args[i] == "--query" && i + 1 < args.len() {
            query = args[i + 1].clone();
            i += 2;
        } else {
            i += 1;
        }
    }

    if filename.is_empty() || query.is_empty() {
        println!("ERROR: usage: ./dbms --file <file> --query '<command>'");
        process::exit(1);
    }

    let mut meta = load_meta(&filename);
    if meta.array_name.is_empty() { meta.array_name = "myarray".into(); }
    if meta.singly_name.is_empty() { meta.singly_name = "mylist".into(); }
    if meta.doubly_name.is_empty() { meta.doubly_name = "mydlist".into(); }
    if meta.stack_name.is_empty() { meta.stack_name = "mystack".into(); }
    if meta.queue_name.is_empty() { meta.queue_name = "myqueue".into(); }
    if meta.tree_name.is_empty() { meta.tree_name = "mytree".into(); }
    if meta.set_name.is_empty() { meta.set_name = "myset".into(); }
    if meta.bst_name.is_empty() { meta.bst_name = "mybst".into(); }
    if meta.ht_name.is_empty() { meta.ht_name = "myht".into(); }

    let mut c = Containers::new(&meta);
    load_containers(&mut c, &filename);

    let pc = parse_command(&query);
    let res = execute_command(&mut c, &pc);

    if res.ok && is_mutating(&pc.command) {
        save_containers(&c, &filename, &meta);
    }

    println!("{}", res.output);

    process::exit(if res.ok { 0 } else { 1 });
}