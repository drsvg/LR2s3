use crate::common::*;
use crate::array::Array;
use std::collections::HashSet;

pub fn print_all_subsets(a: &Array) -> CommandResult {
    if a.size() == 0 { return make_ok("[{}]"); }
    if a.size() > 20 {
        return make_error("ERROR: array too large for subsets (max 20 elements)");
    }

    let mut unique: HashSet<String> = HashSet::new();
    let total: u64 = 1u64 << a.size();
    for mask in 0..total {
        let mut parts = Vec::new();
        for i in 0..a.size() {
            if mask & (1u64 << i) != 0 {
                parts.push(a.data[i].clone());
            }
        }
        unique.insert(format!("{{{}}}", parts.join(", ")));
    }

    let mut result: Vec<String> = unique.into_iter().collect();
    result.sort_by(|a, b| {
        if a.len() != b.len() { a.len().cmp(&b.len()) } else { a.cmp(b) }
    });

    make_ok(format!("[{}]", result.join(", ")))
}