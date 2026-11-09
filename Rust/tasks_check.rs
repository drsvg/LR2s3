use crate::common::*;
use std::collections::{HashMap, HashSet, VecDeque};

pub fn check_task_order(
    tasks: &[String],
    dependencies: &[(String, String)],
) -> CommandResult {
    let known: HashSet<&String> = tasks.iter().collect();
    let mut indeg: HashMap<String, i32> = HashMap::new();
    let mut adj: HashMap<String, Vec<String>> = HashMap::new();

    for t in tasks {
        indeg.insert(t.clone(), 0);
    }
    for (task, dep) in dependencies {
        if !known.contains(task) || !known.contains(dep) {
            return make_error("ERROR: unknown task name in dependencies");
        }
        adj.entry(dep.clone()).or_default().push(task.clone());
        *indeg.entry(task.clone()).or_insert(0) += 1;
    }

    let mut stack: Vec<String> = tasks.iter()
        .filter(|t| *indeg.get(*t).unwrap_or(&0) == 0)
        .cloned()
        .collect();

    let mut done = 0;
    while let Some(cur) = stack.pop() {
        done += 1;
        if let Some(neighbors) = adj.get(&cur) {
            for nxt in neighbors {
                let e = indeg.get_mut(nxt).unwrap();
                *e -= 1;
                if *e == 0 { stack.push(nxt.clone()); }
            }
        }
    }

    make_ok(if done == tasks.len() { "TRUE" } else { "FALSE" })
}