LANG=C  # say "Entering directory" in English for some editors

.PHONY: all
all:
	make -C sample $@
	make -C test $@

.PHONY: check
check:
	make -C sample $@
	make -C test $@

.PHONY: clean
clean:
	make -C sample $@
	make -C test $@

.PHONY: depends
depends:
	make -C sample $@
	make -C test $@
