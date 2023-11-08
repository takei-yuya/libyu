LANG=C  # say "Entering directory" in English for some editors

subdirs := test sample app

.PHONY: all check clean depends
all check clean depends: $(subdirs)

.PHONY: $(subdirs)
$(subdirs):
	make -C $@ $(MAKECMDGOALS)
