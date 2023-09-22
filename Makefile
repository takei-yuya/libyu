LANG=C  # say "Entering directory" in English for some editors

.PHONY: all
all:
	make -C sample $@
	make -C test $@
	make -C app $@

.PHONY: check
check:
	make -C sample $@
	make -C test $@
	make -C app $@

.PHONY: clean
clean:
	make -C sample $@
	make -C test $@
	make -C app $@

.PHONY: depends
depends:
	make -C sample $@
	make -C test $@
	make -C app $@
