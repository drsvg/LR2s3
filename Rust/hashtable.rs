use crate::common::*;

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum ProbeMode { Linear, Double }

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum SlotState { Empty, Occupied, Deleted }

#[derive(Clone)]
pub struct Slot {
    pub key: String,
    pub value: i32,
    pub state: SlotState,
}

pub struct HashTable {
    pub name: String,
    pub slots: Vec<Slot>,
    pub capacity: usize,
    pub size: usize,
    pub deleted_count: usize,
    pub mode: ProbeMode,
}

fn h1(s: &str) -> u64 {
    let mut h: u64 = 1469598103934665603;
    for &c in s.as_bytes() {
        h ^= c as u64;
        h = h.wrapping_mul(1099511628211);
    }
    h
}

fn h2(s: &str, capacity: usize) -> usize {
    let mut h: u64 = 0;
    for &c in s.as_bytes() {
        h = h.wrapping_mul(131).wrapping_add(c as u64);
    }
    if capacity > 1 { 1 + (h % (capacity as u64 - 1)) as usize } else { 1 }
}

impl HashTable {
    pub fn new(name: &str, mode: ProbeMode, capacity: usize) -> Self {
        let slots = vec![Slot { key: String::new(), value: 0, state: SlotState::Empty }; capacity];
        HashTable {
            name: name.to_string(),
            slots,
            capacity,
            size: 0,
            deleted_count: 0,
            mode,
        }
    }

    pub fn clear(&mut self) {
        self.slots.clear();
        self.capacity = 0;
        self.size = 0;
        self.deleted_count = 0;
    }

    fn probe_index(&self, value: &str, i: usize) -> usize {
        let base = (h1(value) % self.capacity as u64) as usize;
        match self.mode {
            ProbeMode::Linear => (base + i) % self.capacity,
            ProbeMode::Double => {
                let step = h2(value, self.capacity);
                (base + i * step) % self.capacity
            }
        }
    }

    pub fn insert(&mut self, value: &str) -> CommandResult {
        let mut first_deleted: Option<usize> = None;
        for i in 0..self.capacity {
            let idx = self.probe_index(value, i);
            match self.slots[idx].state {
                SlotState::Occupied if self.slots[idx].key == value => {
                    return make_ok(value);
                }
                SlotState::Deleted if first_deleted.is_none() => {
                    first_deleted = Some(idx);
                }
                SlotState::Empty => {
                    let target = first_deleted.unwrap_or(idx);
                    if self.slots[target].state == SlotState::Deleted {
                        self.deleted_count -= 1;
                    }
                    self.slots[target].key = value.to_string();
                    self.slots[target].state = SlotState::Occupied;
                    self.size += 1;
                    return make_ok(value);
                }
                _ => {}
            }
        }
        if let Some(t) = first_deleted {
            self.slots[t].key = value.to_string();
            self.slots[t].state = SlotState::Occupied;
            self.deleted_count -= 1;
            self.size += 1;
            return make_ok(value);
        }
        make_error("ERROR: hash table is full")
    }

    pub fn find(&self, value: &str) -> CommandResult {
        for i in 0..self.capacity {
            let idx = self.probe_index(value, i);
            match self.slots[idx].state {
                SlotState::Empty => return make_ok("FALSE"),
                SlotState::Occupied if self.slots[idx].key == value => return make_ok("TRUE"),
                _ => {}
            }
        }
        make_ok("FALSE")
    }

    pub fn remove(&mut self, value: &str) -> CommandResult {
        for i in 0..self.capacity {
            let idx = self.probe_index(value, i);
            match self.slots[idx].state {
                SlotState::Empty => return make_error("ERROR: value not found"),
                SlotState::Occupied if self.slots[idx].key == value => {
                    self.slots[idx].state = SlotState::Deleted;
                    self.size -= 1;
                    self.deleted_count += 1;
                    return make_ok(value);
                }
                _ => {}
            }
        }
        make_error("ERROR: value not found")
    }

    pub fn print(&self) -> CommandResult {
        let items: Vec<String> = self.slots.iter()
            .filter(|s| s.state == SlotState::Occupied)
            .map(|s| s.key.clone())
            .collect();
        make_ok(format!("{{{}}}", items.join(", ")))
    }

    pub fn stats(&self) -> CommandResult {
        let mut occupied = 0;
        let mut deleted = 0;
        let mut empty = 0;
        let mut max_run = 0usize;
        let mut cur_run = 0usize;
        for s in &self.slots {
            match s.state {
                SlotState::Occupied => { occupied += 1; cur_run += 1; }
                _ => {
                    max_run = max_run.max(cur_run);
                    cur_run = 0;
                    if s.state == SlotState::Deleted { deleted += 1; } else { empty += 1; }
                }
            }
        }
        max_run = max_run.max(cur_run);
        let mode = if self.mode == ProbeMode::Linear { "linear" } else { "double" };
        let lf = self.size as f64 / self.capacity as f64;
        make_ok(format!(
            "mode={} size={} occupied={} deleted={} empty={} load_factor={} max_cluster_run={}",
            mode, self.size, occupied, deleted, empty, lf, max_run
        ))
    }

    fn insert_with_value(&mut self, value: &str, int_val: i32) -> CommandResult {
        let mut first_deleted: Option<usize> = None;
        for i in 0..self.capacity {
            let idx = self.probe_index(value, i);
            match self.slots[idx].state {
                SlotState::Occupied if self.slots[idx].key == value => {
                    self.slots[idx].value = int_val;
                    return make_ok(value);
                }
                SlotState::Deleted if first_deleted.is_none() => { first_deleted = Some(idx); }
                SlotState::Empty => {
                    let target = first_deleted.unwrap_or(idx);
                    if self.slots[target].state == SlotState::Deleted { self.deleted_count -= 1; }
                    self.slots[target].key = value.to_string();
                    self.slots[target].value = int_val;
                    self.slots[target].state = SlotState::Occupied;
                    self.size += 1;
                    return make_ok(value);
                }
                _ => {}
            }
        }
        make_error("ERROR: hash table is full")
    }

    fn find_with_value(&self, value: &str) -> Option<i32> {
        for i in 0..self.capacity {
            let idx = self.probe_index(value, i);
            match self.slots[idx].state {
                SlotState::Empty => return None,
                SlotState::Occupied if self.slots[idx].key == value => {
                    return Some(self.slots[idx].value);
                }
                _ => {}
            }
        }
        None
    }
}

pub fn ht_longest_unique(base_mode: ProbeMode, s: &str) -> CommandResult {
    if s.is_empty() { return make_ok("0 ()"); }

    let mut ht = HashTable::new("temp", base_mode, 257);
    let bytes = s.as_bytes();
    let mut max_len = 0usize;
    let mut max_start = 0usize;
    let mut left = 0usize;

    for right in 0..bytes.len() {
        let ch = (bytes[right] as char).to_string();
        if let Some(prev) = ht.find_with_value(&ch) {
            if prev as usize >= left { left = prev as usize + 1; }
        }
        ht.insert_with_value(&ch, right as i32);
        let cur_len = right - left + 1;
        if cur_len > max_len { max_len = cur_len; max_start = left; }
    }

    let longest = &s[max_start..max_start + max_len];
    make_ok(format!("{} ({})", max_len, longest))
}