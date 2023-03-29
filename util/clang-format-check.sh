#! /bin/bash
test "$1" == "--autofix" && AUTOFIX=1 && shift
test "$1" == "--ci" && AUTOFIX=1 && CI=1 && shift

CLANG_FORMAT=${1:-clang-format}

# Starting
echo -n "Running clang-format checks, version: "
${CLANG_FORMAT} --version

if [ 0 != $? ]; then
    echo -e "\033[1;31mclang-format missing: ${CLANG_FORMAT}\033[0m"
    exit 1
fi

git fetch --all
base_commit="origin/dev"
echo "Checking against parent commit $(git rev-parse $base_commit)"

# To simplify CI debugging, tell what files are considered.
echo "--- Listing all changed files:"
git diff --name-only ${base_commit}
echo "---"

FMT_FILE=$(mktemp)

filesToCheck="$(git diff --name-only ${base_commit} | grep -E '[.](cxx|h|C)$' || true)"
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

if test -n "$CI"
then
  if test -n "$fail" ; then
    echo -e "\033[1;31mYou must pass the clang-format checks before submitting a pull request for the files: \033[0m"
    for f in $filesToCheck; do
        d=$(diff -u "$f" <($CLANG_FORMAT -style=file "$f") || true)
	if ! [ -z "$d" ]; then
            echo "${f}"
	fi
    done
    exit 1
  else
    echo -e "\033[1;32m\xE2\x9C\x93 passed clang-format checks\033[0m $1";
  fi
else

  if test -n "$fail"; then
        echo "Clang-format check failed. Try --autofix to fix it in the code."
  fi
fi
