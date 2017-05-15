# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

if(FAIRDB_FOUND)
add_subdirectory (commondb)
endif(FAIRDB_FOUND)

