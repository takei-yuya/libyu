LANG=C  # say "Entering directory" in English for some editors
CXXFLAGS=-Wall -g3
CC=$(CXX)

.PHONY: all
all:
	make -C test $@

.PHONY: check
check:
	make -C test $@

.PHONY: clean
clean:
	make -C test $@

.PHONY: depends
depends:
	make -C test $@
