use crate::common::*;
use std::collections::VecDeque;

const PHANTOM: i32 = -999999;

pub struct TNode {
    pub value: i32,
    pub left: Option<Box<TNode>>,
    pub right: Option<Box<TNode>>,
}

pub struct CBTree {
    pub name: String,
    pub root: Option<Box<TNode>>,
    pub size: usize,
}

impl CBTree {
    pub fn new(name: &str) -> Self {
        CBTree { name: name.to_string(), root: None, size: 0 }
    }

    pub fn clear(&mut self) {
        self.root = None;
        self.size = 0;
    }

    pub fn insert(&mut self, value: i32) -> CommandResult {
        let node = Box::new(TNode { value, left: None, right: None });

        if self.root.is_none() {
            self.root = Some(node);
            self.size += 1;
            return make_ok(value.to_string());
        }

        let mut queue: VecDeque<*mut TNode> = VecDeque::new();
        queue.push_back(self.root.as_mut().unwrap().as_mut());

        while let Some(cur_ptr) = queue.pop_front() {
            let cur = unsafe { &mut *cur_ptr };
            if cur.left.is_none() {
                cur.left = Some(node);
                self.size += 1;
                return make_ok(value.to_string());
            }
            if cur.right.is_none() {
                cur.right = Some(node);
                self.size += 1;
                return make_ok(value.to_string());
            }
            queue.push_back(cur.left.as_mut().unwrap().as_mut());
            queue.push_back(cur.right.as_mut().unwrap().as_mut());
        }
        make_error("ERROR: insertion failed")
    }

    fn find_node(n: &Option<Box<TNode>>, value: i32) -> bool {
        match n {
            None => false,
            Some(node) => {
                if node.value == value {
                    true
                } else {
                    Self::find_node(&node.left, value) || Self::find_node(&node.right, value)
                }
            }
        }
    }

    pub fn get(&self, value: i32) -> CommandResult {
        make_ok(if Self::find_node(&self.root, value) { "TRUE" } else { "FALSE" })
    }

    fn is_complete(n: &Option<Box<TNode>>, index: usize, total: usize) -> bool {
        match n {
            None => true,
            Some(node) => {
                if index >= total {
                    return false;
                }
                if node.value == PHANTOM {
                    return false;
                }
                Self::is_complete(&node.left, 2 * index + 1, total)
                    && Self::is_complete(&node.right, 2 * index + 2, total)
            }
        }
    }

    pub fn check(&self) -> CommandResult {
        let ok = Self::is_complete(&self.root, 0, self.size);
        make_ok(if ok { "TRUE" } else { "FALSE" })
    }

    pub fn insert_empty(&mut self) -> CommandResult {
        if self.root.is_none() {
            return make_ok("EMPTY");
        }

        let mut queue: VecDeque<*mut TNode> = VecDeque::new();
        queue.push_back(self.root.as_mut().unwrap().as_mut());

        while let Some(cur_ptr) = queue.pop_front() {
            let cur = unsafe { &mut *cur_ptr };
            if cur.left.is_none() {
                cur.left = Some(Box::new(TNode { value: PHANTOM, left: None, right: None }));
                self.size += 1;
                return make_ok("EMPTY");
            }
            if cur.right.is_none() {
                cur.right = Some(Box::new(TNode { value: PHANTOM, left: None, right: None }));
                self.size += 1;
                return make_ok("EMPTY");
            }
            queue.push_back(cur.left.as_mut().unwrap().as_mut());
            queue.push_back(cur.right.as_mut().unwrap().as_mut());
        }
        make_error("ERROR: tree is full")
    }

    fn print_node(n: &Option<Box<TNode>>, out: &mut Vec<String>) {
        if let Some(node) = n {
            if node.value == PHANTOM {
                return;
            }
            out.push(node.value.to_string());
            Self::print_node(&node.left, out);
            Self::print_node(&node.right, out);
        }
    }

    pub fn print(&self) -> CommandResult {
        let mut items = Vec::new();
        Self::print_node(&self.root, &mut items);
        make_ok(format!("[{}]", items.join(" ")))
    }
}