#!/bin/bash
find  -type f | grep -e '.(\.C\|\.cpp\|\.cxx\|\.h)$'  | grep -v ^./macros | xargs -L 1 clang-format -i 
echo "Use git add -A ; git commit -m \"clang-format all files\" --author=\"white space <whitespace@example.com>\" to commit changes."
