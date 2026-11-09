pub const MAX_ARRAY_SIZE: usize = 10000;
pub const SET_BUCKETS: usize = 1021;

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum ContainerType {
    Array,
    Singly,
    Doubly,
    Stack,
    Queue,
    Tree,
    Unknown,
}

#[derive(Debug, Clone)]
pub struct CommandResult {
    pub ok: bool,
    pub output: String,
}

pub fn make_ok(out: impl Into<String>) -> CommandResult {
    CommandResult { ok: true, output: out.into() }
}

pub fn make_error(msg: impl Into<String>) -> CommandResult {
    CommandResult { ok: false, output: msg.into() }
}