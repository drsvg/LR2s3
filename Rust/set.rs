use crate::common::*;
use std::collections::HashSet;

pub struct Set {
    pub name: String,
    pub items: HashSet<String>,
}

impl Set {
    pub fn new(name: &str) -> Self {
        Set { name: name.to_string(), items: HashSet::new() }
    }
    pub fn clear(&mut self) { self.items.clear(); }
    pub fn add(&mut self, value: &str) -> CommandResult {
        self.items.insert(value.to_string()); make_ok(value)
    }
    pub fn del(&mut self, value: &str) -> CommandResult {
        if self.items.remove(value) { make_ok(value) }
        else { make_error("ERROR: value not found") }
    }
    pub fn at(&self, value: &str) -> CommandResult {
        make_ok(if self.items.contains(value) { "TRUE" } else { "FALSE" })
    }
    pub fn print(&self) -> CommandResult {
        let mut items: Vec<&String> = self.items.iter().collect();
        items.sort();
        make_ok(format!("{{{}}}", items.iter().map(|s| s.as_str()).collect::<Vec<_>>().join(", ")))
    }
}