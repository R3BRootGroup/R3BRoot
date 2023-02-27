#! /bin/bash
test "$1" == "--autofix" && AUTOFIX=1 && shift
test "$1" == "--ci" && AUTOFIX=1 && CI=1 && shift

CLANG_FORMAT=${1:-clang-format}

echo -n "Running clang-format checks, version: "
${CLANG_FORMAT} --version

if [ 0 != $? ]; then
    echo -e "\033[1;31mclang-format missing: ${CLANG_FORMAT}\033[0m"
    exit 1
fi

if [ "$TRAVIS" != "true" ] ; then
  # Not in a pull request, so compare against parent commit
  base_commit="origin/dev"
  echo "Checking against parent commit $(git rev-parse $base_commit)"
else
  base_commit="$TRAVIS_COMMIT_RANGE"
  echo "Checking against commit $base_commit"
fi

# To simplify CI debugging, tell what files are considered.
echo "--- Listing all changed files:"
git diff --name-only ${base_commit}
echo "---"

FMT_FILE=$(mktemp)

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
            echo "$d" | tee -a ${FMT_FILE}
            fail=1
	fi
    fi
done



if  test -n "$CI"
then
    if test -n "$(git status --porcelain)" ; then
    echo -e "\033[1;31mYou must pass the clang-format checks before submitting a pull request.\033[0m"
    echo "Changes:"
    git diff
    echo "Trying to upload clang-format patch here:"
    git diff | curl -F 'sprunge=<-' http://sprunge.us
    exit 1
    else
        echo -e "\033[1;32m\xE2\x9C\x93 passed clang-format checks\033[0m $1";
    fi
else
    if test -n "$fail"
    then
        echo "Clang-format check failed. Try --autofix to fix it in the code."
    fi
fi
