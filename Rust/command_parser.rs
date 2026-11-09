use crate::common::*;

#[derive(Debug)]
pub struct ParsedCommand {
    pub raw: String,
    pub tokens: Vec<String>,
    pub container_type: ContainerType,
    pub command: String,
}

pub fn split_tokens(s: &str) -> Vec<String> {
    s.split_whitespace().map(|t| t.to_string()).collect()
}

pub fn container_type_from_command(command: &str) -> ContainerType {
    let mut chars = command.chars();
    match chars.next() {
        Some('M') => ContainerType::Array,
        Some('F') => ContainerType::Singly,
        Some('L') => ContainerType::Doubly,
        Some('S') => ContainerType::Stack,
        Some('Q') => ContainerType::Queue,
        Some('T') => ContainerType::Tree,
        _ => ContainerType::Unknown,
    }
}

pub fn parse_command(query: &str) -> ParsedCommand {
    let tokens = split_tokens(query);
    let command = tokens.first().cloned().unwrap_or_default();
    let container_type = container_type_from_command(&command);
    ParsedCommand { raw: query.to_string(), tokens, container_type, command }
}