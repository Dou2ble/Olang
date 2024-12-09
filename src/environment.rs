use crate::builtin;
use crate::value::{Function, Value};
use std::collections::HashMap;

pub struct Environment {
    scopes: Vec<HashMap<String, Value>>,
}

impl Environment {
    pub fn new() -> Self {
        Environment {
            scopes: vec![HashMap::new()],
        }
    }

    // remove a scope to the environment
    pub fn pop(&mut self) -> &mut Self {
        self.scopes.pop();
        self
    }
    // add a scope to the environment
    pub fn push(&mut self) -> &mut Self {
        self.scopes.push(HashMap::new());
        self
    }

    pub fn get(&self, id: &String) -> Option<Value> {
        for value in self.scopes.iter().rev() {
            match value.get(id) {
                Some(v) => {
                    return Some(v.clone());
                }
                _ => {}
            }
        }

        None
    }

    pub fn declare(&mut self, id: String, value: Value) -> &mut Self {
        self.scopes.last_mut().unwrap().insert(id, value);
        self
    }

    fn declareBuiltin(&mut self, id: String, function: fn(Vec<Value>) -> Value) -> &mut Self {
        self.declare(id, Value::Function(Function::Builtin(function)));
        self
    }
}

impl Default for Environment {
    fn default() -> Self {
        let mut env = Environment::new();
        env.declare(
            "printLn".to_string(),
            Value::Function(Function::Builtin(builtin::print_ln)),
        )
        .declare(
            "toString".to_string(),
            Value::Function(Function::Builtin(builtin::to_string)),
        );
        env
    }
}
