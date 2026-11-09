use crate::common::*;

pub struct Stack {
    pub name: String,
    pub items: Vec<String>,
}

impl Stack {
    pub fn new(name: &str) -> Self {
        Stack { name: name.to_string(), items: Vec::new() }
    }
    pub fn clear(&mut self) { self.items.clear(); }
    pub fn push(&mut self, value: &str) -> CommandResult {
        self.items.push(value.to_string()); make_ok(value)
    }
    pub fn pop(&mut self) -> CommandResult {
        match self.items.pop() {
            Some(v) => make_ok(v),
            None => make_error("ERROR: stack is empty"),
        }
    }
    pub fn print(&self) -> CommandResult {
        let reversed: Vec<&String> = self.items.iter().rev().collect();
        make_ok(format!("[{}]", reversed.iter().map(|s| s.as_str()).collect::<Vec<_>>().join(", ")))
    }
}