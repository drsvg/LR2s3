use crate::common::*;

pub struct SinglyList {
    pub name: String,
    pub items: Vec<String>,
}

impl SinglyList {
    pub fn new(name: &str) -> Self {
        SinglyList { name: name.to_string(), items: Vec::new() }
    }
    pub fn clear(&mut self) { self.items.clear(); }

    pub fn push_head(&mut self, value: &str) -> CommandResult {
        self.items.insert(0, value.to_string()); make_ok(value)
    }
    pub fn push_tail(&mut self, value: &str) -> CommandResult {
        self.items.push(value.to_string()); make_ok(value)
    }
    pub fn push_after(&mut self, value: &str, target: &str) -> CommandResult {
        match self.items.iter().position(|x| x == target) {
            Some(i) => { self.items.insert(i + 1, value.to_string()); make_ok(value) }
            None => make_error("ERROR: target not found"),
        }
    }
    pub fn push_before(&mut self, value: &str, target: &str) -> CommandResult {
        match self.items.iter().position(|x| x == target) {
            Some(i) => { self.items.insert(i, value.to_string()); make_ok(value) }
            None => make_error("ERROR: target not found"),
        }
    }
    pub fn del_head(&mut self) -> CommandResult {
        if self.items.is_empty() { return make_error("ERROR: list is empty"); }
        make_ok(self.items.remove(0))
    }
    pub fn del_tail(&mut self) -> CommandResult {
        if self.items.is_empty() { return make_error("ERROR: list is empty"); }
        make_ok(self.items.pop().unwrap())
    }
    pub fn del_value(&mut self, value: &str) -> CommandResult {
        match self.items.iter().position(|x| x == value) {
            Some(i) => make_ok(self.items.remove(i)),
            None => make_error("ERROR: value not found"),
        }
    }
    pub fn del_near(&mut self, target: &str, is_after: bool) -> CommandResult {
        match self.items.iter().position(|x| x == target) {
            Some(i) => {
                if is_after {
                    if i + 1 < self.items.len() { make_ok(self.items.remove(i + 1)) }
                    else { make_error("ERROR: nothing to delete after tail") }
                } else {
                    if i == 0 { return make_error("ERROR: nothing to delete before head"); }
                    make_ok(self.items.remove(i - 1))
                }
            }
            None => make_error("ERROR: target not found"),
        }
    }
    pub fn get(&self) -> CommandResult {
        make_ok(format!("[{}]", self.items.join(", ")))
    }
    pub fn find(&self, value: &str) -> CommandResult {
        make_ok(if self.items.iter().any(|x| x == value) { "TRUE" } else { "FALSE" })
    }
    pub fn print(&self) -> CommandResult { self.get() }
}