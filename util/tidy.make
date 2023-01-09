SHELL='/bin/bash'

ALL=$(shell grep -oE 'R3BRoot[A-Za-z0-9_/.]*cxx' compile_commands.json | grep -v build | sed -E 's@R3BRoot[^/]*/@@' | sed -E 's/cxx$$/tidy/' | sed -E 's@^@'$$(pwd)/'@' )

OBJDIR=$(shell readlink -f . )
SRCDIR=$(shell readlink -f .. )

#FIX="--fix"

tidy.txt: $(ALL)
	@cat $(ALL) | ( grep -vE 'warnings generated|Suppressed' || true ) > $@
	@echo "Wrote $@"

$(OBJDIR)/%.tidy: $(SRCDIR)/%.cxx  #$(SRCDIR)/%.h
	@mkdir -p $(shell dirname $@)
	clang-tidy $(FIX) -p $(OBJDIR)  -header-filter=.*  $^ 2>&1 | tee $@
#-system-headers

tidy-clean:
	@rm -f tidy.txt $(ALL)
