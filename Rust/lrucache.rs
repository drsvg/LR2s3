use crate::common::*;
use std::collections::HashMap;

pub struct LRUCache {
    pub capacity: usize,
    pub order: Vec<String>,         
    pub map: HashMap<String, String>,
    pub initialized: bool,
}

impl LRUCache {
    pub fn new() -> Self {
        LRUCache { capacity: 0, order: Vec::new(), map: HashMap::new(), initialized: false }
    }

    pub fn init(&mut self, capacity: i32) {
        if capacity <= 0 { return; }
        self.capacity = capacity as usize;
        self.order.clear();
        self.map.clear();
        self.initialized = true;
    }

    pub fn clear(&mut self) {
        self.order.clear();
        self.map.clear();
        self.capacity = 0;
        self.initialized = false;
    }

    pub fn set(&mut self, key: &str, value: &str) -> CommandResult {
        if self.map.contains_key(key) {
            self.map.insert(key.to_string(), value.to_string());
            self.order.retain(|k| k != key);
            self.order.insert(0, key.to_string());
            return make_ok(value);
        }
        if self.map.len() >= self.capacity {
            if let Some(evicted) = self.order.pop() {
                self.map.remove(&evicted);
            }
        }
        self.map.insert(key.to_string(), value.to_string());
        self.order.insert(0, key.to_string());
        make_ok(value)
    }

    pub fn get(&mut self, key: &str) -> CommandResult {
        match self.map.get(key) {
            Some(v) => {
                let v = v.clone();
                self.order.retain(|k| k != key);
                self.order.insert(0, key.to_string());
                make_ok(v)
            }
            None => make_ok("-1"),
        }
    }

    pub fn print(&self) -> CommandResult {
        let items: Vec<String> = self.order.iter()
            .map(|k| format!("{}->{}", k, self.map.get(k).unwrap()))
            .collect();
        make_ok(format!("cap={} size={} [{}]", self.capacity, self.map.len(), items.join(", ")))
    }
}