#!/bin/bash
git status | grep "working tree clean" || { echo "Please run me from a clean working tree." ; exit 1 ;  }
test -z "$1" &&  { echo "Usage: $0 classname" ; exit 1 ; }
# First, find all uses which do not declare class FooBar or #include <FooBar.h>
# From these, take the ones who actually use FooBar
# Finally, insert an #include for <FooBar.h> before the first #include.
find . -iname \*.cxx -or -iname \*.h \
    | xargs grep -L -E "class * $1 *;|$1.h" \
    | xargs grep -l $1 \
    | xargs -L 1 sed '0,/#include/s/#include/#include <'$1'.h>\n#include/' -i
