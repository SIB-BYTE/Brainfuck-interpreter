use std::{
    fs,
    io,
    io::Read,
    collections::HashMap,
};

// Maps a file into a string buffer and returns it:
pub fn map_file(file: String) -> String {
    match fs::read_to_string(file) {
        Ok(file_buffer) => file_buffer,
        Err(_) => panic!("Can't map file into memory!"),
    }
}

// Preprocessing function that generates & returns a hashmap of positions:
pub fn construct_jump_table(instruction_vector: &Vec<char>) -> HashMap<usize, usize> {
    let mut table_stack:  Vec<usize> = Vec::new(); // Used for constructing the table
    let mut balance_stack: Vec<char> = Vec::new(); // Used for balancing of '[' && ']'
    let mut table: HashMap<usize, usize> = HashMap::new(); // Table to store position mappings

    for (pos, curr_char) in instruction_vector.iter().enumerate() {
        match curr_char {
            '[' => {
                // Balancing stack:
                balance_stack.push(*curr_char);

                // Jump table code:
                table_stack.push(pos);
            },

            ']' => {
                // Balancing stack code:
                if balance_stack.last() == None { panic!("Imbalanced brackets!"); }
                balance_stack.pop();

                // Jump table code:
                let top_value = table_stack.last().cloned().unwrap();
                table_stack.pop();
                table.insert(top_value, pos);
                table.insert(pos, top_value);
            },

            _ => { /* Get next iteration */},
        }
    }

    if balance_stack.len() != 0 { panic!("Imbalanced brackets!"); }

    table
}

pub fn execute(instruction_list: String) {
    if instruction_list.len() == 0 { panic!("Empty file"); }

    // Vector representation of the mapped file contents:
    let instruction_vector: Vec<char> = instruction_list.chars().collect();

    // Program tape:
    let mut program: [u8 ; 30_000]= [b'\0'; 30_000];
    
    // Bracket table construction for the instruction vector:
    let bracket_table: HashMap<usize, usize> = construct_jump_table(&instruction_vector);

    // Program cell index:
    let mut cell_idx: usize = 0;
    
    // Program counter / current index into instruction_vector:
    let mut pc: usize = 0;
    
    // Interpretation loop:
    while pc < instruction_vector.len() {
        match instruction_vector[pc] {
            '<' => {
                if cell_idx == 0 { panic!("[Error] At starting cell, cannot go backwards!") }
                cell_idx -= 1;
            }

            '>' => {
                if cell_idx == 30_000 { panic!("[Error] At ending cell, cannot go forwards!") }
                cell_idx += 1;
            }

            '+' => { program[cell_idx] += 1; }
            '-' => { program[cell_idx] -= 1; }
            '.' => { print!("{}", program[cell_idx] as char); }
            ',' => {
                program[cell_idx] = io::stdin()
                                        .bytes()
                                        .next()
                                        .and_then(|result| result.ok())
                                        .map(|byte| byte as u8)
                                        .unwrap();
            }

            '[' => { if program[cell_idx] == 0 { pc = bracket_table[&pc]; } }
            ']' => { if program[cell_idx] != 0 { pc = bracket_table[&pc]; } }
            _ => { }
        }

        pc += 1;
    }
}
