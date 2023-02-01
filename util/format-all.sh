#!/bin/bash
find  -type f | grep -e '.(\.C\|\.cpp\|\.cxx\|\.h)$'  | grep -v ^./macros | grep -v ^./build | xargs -L 1 -P 10 sed $'s/\r$//' -i
find  -type f | grep -e '.(\.C\|\.cpp\|\.cxx\|\.h)$'  | grep -v ^./macros | grep -v ^./build |xargs -L 1 -P 10 clang-format -i 
echo "Use git add -A ; git commit -m \"clang-format all files\" --author=\"white space <whitespace@example.com>\" to commit changes."
