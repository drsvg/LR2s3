use crate::common::*;

pub struct Queue {
    pub name: String,
    pub items: Vec<String>,
}

impl Queue {
    pub fn new(name: &str) -> Self {
        Queue { name: name.to_string(), items: Vec::new() }
    }
    pub fn clear(&mut self) { self.items.clear(); }
    pub fn push(&mut self, value: &str) -> CommandResult {
        self.items.push(value.to_string()); make_ok(value)
    }
    pub fn pop(&mut self) -> CommandResult {
        if self.items.is_empty() { return make_error("ERROR: queue is empty"); }
        make_ok(self.items.remove(0))
    }
    pub fn print(&self) -> CommandResult {
        make_ok(format!("[{}]", self.items.join(", ")))
    }
}