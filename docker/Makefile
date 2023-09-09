DOCKERFILES=$(wildcard */Dockerfile)
DOCKERIMAGES=$(DOCKERFILES:%/Dockerfile=%)

.PHONY: all
all: build

.PHONY: build
build:
	$(foreach name,$(DOCKERIMAGES),docker build -t takei/libyu-dev:$(name) $(name);)

.PHONY: push
push:
	$(foreach name,$(DOCKERIMAGES),docker push takei/libyu-dev:$(name);)

.PHONY: pull
	$(foreach name,$(DOCKERIMAGES),docker pull takei/libyu-dev:$(name);)

