use crate::common::*;

#[derive(Default)]
pub struct BSTNode {
    pub value: i32,
    pub left: Option<Box<BSTNode>>,
    pub right: Option<Box<BSTNode>>,
}

pub struct BST {
    pub name: String,
    pub root: Option<Box<BSTNode>>,
    pub size: usize,
}

impl BST {
    pub fn new(name: &str) -> Self {
        BST { name: name.to_string(), root: None, size: 0 }
    }

    pub fn clear(&mut self) {
        self.root = None;
        self.size = 0;
    }

    fn insert_node(node: &mut Option<Box<BSTNode>>, value: i32) -> bool {
        match node {
            None => {
                *node = Some(Box::new(BSTNode { value, left: None, right: None }));
                true
            }
            Some(n) => {
                if value == n.value {
                    false
                } else if value < n.value {
                    Self::insert_node(&mut n.left, value)
                } else {
                    Self::insert_node(&mut n.right, value)
                }
            }
        }
    }

    pub fn insert(&mut self, value: i32) -> CommandResult {
        if Self::insert_node(&mut self.root, value) {
            self.size += 1;
        }
        make_ok(value.to_string())
    }

    fn preorder_node(n: &Option<Box<BSTNode>>, out: &mut String) {
        if let Some(node) = n {
            out.push_str(&node.value.to_string());
            out.push(',');
            Self::preorder_node(&node.left, out);
            Self::preorder_node(&node.right, out);
        }
    }

    pub fn preorder(&self) -> String {
        let mut out = String::new();
        Self::preorder_node(&self.root, &mut out);
        out
    }

    fn height(n: &Option<Box<BSTNode>>) -> i32 {
        match n {
            None => 0,
            Some(node) => 1 + Self::height(&node.left).max(Self::height(&node.right)),
        }
    }

    pub fn height_result(&self) -> CommandResult {
        make_ok(Self::height(&self.root).to_string())
    }

    fn inorder(n: &Option<Box<BSTNode>>, out: &mut Vec<String>) {
        if let Some(node) = n {
            Self::inorder(&node.left, out);
            out.push(node.value.to_string());
            Self::inorder(&node.right, out);
        }
    }

    pub fn print(&self) -> CommandResult {
        let mut items = Vec::new();
        Self::inorder(&self.root, &mut items);
        make_ok(format!("[{}]", items.join(" ")))
    }
}