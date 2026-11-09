use crate::common::*;
use std::fs::File;
use std::io::{BufRead, BufReader};

#[derive(Default, Clone)]
pub struct StorageMeta {
    pub array_name: String,
    pub singly_name: String,
    pub doubly_name: String,
    pub stack_name: String,
    pub queue_name: String,
    pub tree_name: String,
    pub set_name: String,
    pub bst_name: String,
    pub ht_name: String,
}

pub fn extract_value(line: &str, key: &str) -> String {
    let search = format!("\"{}\"", key);
    let kpos = match line.find(&search) { Some(p) => p, None => return String::new() };
    let colon = match line[kpos + search.len()..].find(':') {
        Some(p) => kpos + search.len() + p,
        None => return String::new(),
    };
    let first_q = match line[colon..].find('"') { Some(p) => colon + p, None => return String::new() };
    let second_q = match line[first_q + 1..].find('"') { Some(p) => first_q + 1 + p, None => return String::new() };
    line[first_q + 1..second_q].to_string()
}

pub fn split_csv(csv: &str, keep_empty: bool) -> Vec<String> {
    let mut result = Vec::new();
    let mut current = String::new();
    for ch in csv.chars() {
        if ch == ',' {
            if !current.is_empty() || keep_empty {
                result.push(current.clone());
            }
            current.clear();
        } else {
            current.push(ch);
        }
    }
    if !current.is_empty() {
        result.push(current);
    } else if keep_empty && !csv.is_empty() && !csv.ends_with(',') {
        result.push(current);
    }
    result
}

fn extract_name(line: &str) -> Option<String> {
    let pipe = line.find('|')?;
    let quote = line.find('"')?;
    if pipe > quote { Some(line[quote + 1..pipe].to_string()) } else { None }
}

pub fn load_meta(filename: &str) -> StorageMeta {
    let mut meta = StorageMeta::default();
    let file = match File::open(filename) { Ok(f) => f, Err(_) => return meta };
    for line in BufReader::new(file).lines().flatten() {
        if line.contains("|array") { if let Some(n) = extract_name(&line) { meta.array_name = n; } }
        else if line.contains("|flist") { if let Some(n) = extract_name(&line) { meta.singly_name = n; } }
        else if line.contains("|dlist") { if let Some(n) = extract_name(&line) { meta.doubly_name = n; } }
        else if line.contains("|stack") { if let Some(n) = extract_name(&line) { meta.stack_name = n; } }
        else if line.contains("|queue") { if let Some(n) = extract_name(&line) { meta.queue_name = n; } }
        else if line.contains("|tree") { if let Some(n) = extract_name(&line) { meta.tree_name = n; } }
        else if line.contains("|set") { if let Some(n) = extract_name(&line) { meta.set_name = n; } }
        else if line.contains("|bst") { if let Some(n) = extract_name(&line) { meta.bst_name = n; } }
        else if line.contains("|ht") { if let Some(n) = extract_name(&line) { meta.ht_name = n; } }
    }
    meta
}