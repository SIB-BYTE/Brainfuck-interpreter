use std::env;
mod interpreter;

fn main() {
    let file = match env::args().nth(1) {
        Some(valid_file) =>  valid_file,
        None => panic!("File doesn't exist")
    };
    
    interpreter::execute(interpreter::map_file(file));
}

