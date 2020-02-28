#! /bin/bash
test "$1" == "--autofix" && AUTOFIX=1 && shift

CLANG_FORMAT=${1:-clang-format}

echo -n "Running clang-format checks, version: "
${CLANG_FORMAT} --version

if [ 0 != $? ]; then
    echo -e "\033[1;31mclang-format missing: ${CLANG_FORMAT}\033[0m"
    exit 1
fi

if [ "$TRAVIS" != "true" ] ; then
  # Not in a pull request, so compare against parent commit
  base_commit="HEAD^"
  echo "Checking against parent commit $(git rev-parse $base_commit)"
else
  base_commit="$TRAVIS_COMMIT_RANGE"
  echo "Checking against commit $base_commit"
fi

filesToCheck="$(git diff --name-only ${base_commit} | grep -e '.(\.C\|\.cpp\|\.cxx\|\.h)$' || true)"
for f in $filesToCheck; do
    if test -n "$AUTOFIX"
    then
	echo "fixing $f, if needed." 
	$CLANG_FORMAT -i -style=file "$f"
    else
	echo "  Checking: ${f}"
	d=$(diff -u "$f" <($CLANG_FORMAT -style=file "$f") || true)
	if ! [ -z "$d" ]; then
            echo "$d"
            fail=1
	fi
    fi
done

if [ "$fail" = 1 ]; then
    echo -e "\033[1;31mYou must pass the clang-format checks before submitting a pull request.\033[0m"
    exit 1
fi

echo -e "\033[1;32m\xE2\x9C\x93 passed clang-format checks\033[0m $1";
exit 0
