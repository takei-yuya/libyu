CPPFLAGS=-I..
CXXFLAGS=-g3 -Wall -Wextra -Werror -Wcast-align -Wcast-qual -Wconversion -Wdelete-non-virtual-dtor -Wdouble-promotion -Weffc++ -Wfloat-equal -Wold-style-cast -Woverloaded-virtual -Wpedantic -Wreorder -Wsign-promo -Wsuggest-override -Wswitch-enum
CC=$(CXX)

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%.cpp=%.o)
BINS=$(SRCS:%.cpp=%)

.PHONY: all
all: $(BINS)

.PHONY: check
check: $(BINS)
	./test_runner $(foreach test,$(BINS),$(test) )

.PHONY: clean
clean:
	$(RM) $(OBJS) $(BINS) dep.Makefile

.PHONY: depends
depends: dep.Makefile

dep.Makefile: $(SRCS)
	$(CXX) $(CPPFLAGS) -MM $^ > $@

-include dep.Makefile
