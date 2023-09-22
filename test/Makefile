CPPFLAGS=-I..
CXXFLAGS=-Wall -Wextra -Werror -Wcast-align -Wcast-qual -Wconversion -Wdelete-non-virtual-dtor -Wdouble-promotion -Weffc++ -Wfloat-equal -Wold-style-cast -Woverloaded-virtual -Wpedantic -Wreorder -Wsign-promo -Wsuggest-override -Wswitch-enum
CC=$(CXX)

TEST_SRCS = $(wildcard *_test.cpp)
TEST_OBJS = $(TEST_SRCS:%.cpp=%.o)
TESTS = $(TEST_SRCS:%.cpp=%)

.PHONY: all
all:

.PHONY: check
check: $(TESTS)
	./test_runner $(foreach test,$(TESTS),$(test) )

.PHONY: clean
clean:
	$(RM) $(TESTS) $(TEST_OBJS) dep.Makefile

.PHONY: depends
depends: dep.Makefile

dep.Makefile: $(TEST_SRCS)
	$(CXX) $(CPPFLAGS) -MM $^ > $@

-include dep.Makefile
