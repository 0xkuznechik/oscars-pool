use html_parser::{Dom, Result};
use lazy_static::lazy_static;
use regex::Regex;
use std::fs::File;
use std::io::{Error, Read};

fn PingAndPrint() -> std::result::Result<(), Box<dyn std::error::Error>> {
    let url = String::from("https://www.vegasinsider.com/awards/odds/oscars/");
    let resp = reqwest::blocking::get(url)?
        .text()?;
    println!("{:#?}", resp);
    Ok(())
}

fn PrettyPrintHTML(contents: &String) -> html_parser::Result<()> {
// fn PrettyPrintHTML(contents: &String) {
    assert!(Dom::parse(contents).is_ok());
    let json = Dom::parse(&contents)?.to_json_pretty()?;
    println!("{json}");
    Ok(())
}

fn Process<'a>(words: dyn Iterator<Item = &'a str>) {
    for w in words {
        println!("111{w}");
        if w.eq("2024Best") {
            return;
        }
    }
}

fn main() {
    PingAndPrint();
    /*
    let f: std::result::Result<File, std::io::Error>  = File::open("debug-tokenized.log");
    let c  = match f {
        Ok(mut file) => {
            let mut contents = String::new();
            if let Ok(_) = file.read_to_string(&mut contents) {
                contents
            } else {
                println!("Couldn't read file.");
                return;
            }
        }
        Err(err) => {
            return;
        }
    };

    let mut bit: bool = false;
    let words: Vec<&str> = c.split('\n').collect();
    for w in words.iter() {
        Process(w);
        // println!("{w}");
    }
    */

    // for i in c.split('\n') {
        /*
        if i.eq("2024Best") {
            bit = true;
            Process(i);
        }
        if bit && i.eq("2023") {
            bit = false;
        }
        if bit {
            println!("{i}");
        }
        */
    // }
}
