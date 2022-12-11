#!/bin/bash
if test $# -ne 2
then
    echo "Usage: $0 <oldname> <newname>"
    echo "Where <oldname> is an extended regex and newname is a sed-compatible replacement."
    echo "This will change checked out files without prompting!"
fi

grep -E "$1" -r . --exclude .git -l | xargs -L 1 sed -E "s@$1@$2@g" -i 
