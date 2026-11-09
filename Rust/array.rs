use crate::common::*;

pub struct Array {
    pub name: String,
    pub data: Vec<String>,
}

impl Array {
    pub fn new(name: &str) -> Self {
        Array { name: name.to_string(), data: Vec::new() }
    }

    pub fn clear(&mut self) {
        self.data.clear();
    }

    pub fn size(&self) -> usize {
        self.data.len()
    }

    pub fn push(&mut self, value: &str, index: i32) -> CommandResult {
        if self.data.len() >= MAX_ARRAY_SIZE {
            return make_error("ERROR: array overflow");
        }
        if index == -1 {
            self.data.push(value.to_string());
            return make_ok(value);
        }
        if index < 0 || index as usize > self.data.len() {
            return make_error("ERROR: index out of range");
        }
        self.data.insert(index as usize, value.to_string());
        make_ok(value)
    }

    pub fn del(&mut self, index: i32) -> CommandResult {
        if index < 0 || index as usize >= self.data.len() {
            return make_error("ERROR: index out of range");
        }
        let removed = self.data.remove(index as usize);
        make_ok(removed)
    }

    pub fn get(&self, index: i32) -> CommandResult {
        if index < 0 || index as usize >= self.data.len() {
            return make_error("ERROR: index out of range");
        }
        make_ok(self.data[index as usize].clone())
    }

    pub fn set(&mut self, index: i32, value: &str) -> CommandResult {
        if index < 0 || index as usize >= self.data.len() {
            return make_error("ERROR: index out of range");
        }
        self.data[index as usize] = value.to_string();
        make_ok(value)
    }

    pub fn len(&self) -> CommandResult {
        make_ok(self.data.len().to_string())
    }

    pub fn print(&self) -> CommandResult {
        make_ok(format!("[{}]", self.data.join(", ")))
    }
}