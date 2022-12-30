#!/bin/bash
find  -type f | grep -e '.(\.C\|\.cpp\|\.cxx\|\.h)$'  | grep -v ^./macros | grep -v ^./frs | grep -v ^./sofia | grep -v ^./asyeos | grep -v ^./glad-tpc | xargs -L 1 clang-format-8 -i 
echo "Use git add -A ; git commit -m \"clang-format all files\" --author=\"white space <whitespace@example.com>\" to commit changes."
