CPPFLAGS=-I..
CXXFLAGS=-Wall -g3
CC=$(CXX)

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%.cpp=%.o)
BINS=$(SRCS:%.cpp=%)

.PHONY: all
all: $(BINS)

.PHONY: check
check:

.PHONY: clean
clean:
	$(RM) $(OBJS) $(BINS) dep.Makefile

.PHONY: depends
depends: dep.Makefile

dep.Makefile: $(SRCS)
	$(CXX) $(CPPFLAGS) -MM $^ > $@

-include dep.Makefile
