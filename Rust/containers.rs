use crate::array::Array;
use crate::bst::BST;
use crate::cbtree::CBTree;
use crate::doubly_list::DoublyList;
use crate::hashtable::{HashTable, ProbeMode};
use crate::lrucache::LRUCache;
use crate::queue::Queue;
use crate::set::Set;
use crate::singly_list::SinglyList;
use crate::stack::Stack;
use crate::storage::StorageMeta;

pub struct Containers {
    pub arr: Array,
    pub slist: SinglyList,
    pub dlist: DoublyList,
    pub stk: Stack,
    pub q: Queue,
    pub tree: CBTree,
    pub set: Set,
    pub bst: BST,
    pub ht: HashTable,
    pub lru: LRUCache,
}

impl Containers {
    pub fn new(meta: &StorageMeta) -> Self {
        Containers {
            arr: Array::new(&meta.array_name),
            slist: SinglyList::new(&meta.singly_name),
            dlist: DoublyList::new(&meta.doubly_name),
            stk: Stack::new(&meta.stack_name),
            q: Queue::new(&meta.queue_name),
            tree: CBTree::new(&meta.tree_name),
            set: Set::new(&meta.set_name),
            bst: BST::new(&meta.bst_name),
            ht: HashTable::new(&meta.ht_name, ProbeMode::Linear, 10007),
            lru: LRUCache::new(),
        }
    }
}