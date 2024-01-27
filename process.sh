#!/usr/bin/bash

cargo run > debug.log
sed  's/> </>\n</g' debug.log > debug-edited.log
sed 's/2024 Best/2024Best/g' debug-edited.log | sed 's/ /\n/g' > debug-tokenized.log
bazel run :main > movie-list.txt
cargo clean
bazel clean
