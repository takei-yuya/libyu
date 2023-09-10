CPPFLAGS=-I..
CXXFLAGS=-Wall -g3
CC=$(CXX)

TEST_SRCS = $(wildcard *_test.cpp)
TEST_OBJS = $(TEST_SRCS:%.cpp=%.o)
TESTS = $(TEST_SRCS:%.cpp=%)

.PHONY: all
all:

.PHONY: check
check: $(TESTS)
	$(foreach test,$(TESTS),./$(test);)

.PHONY: clean
clean:
	$(RM) $(TESTS) $(TEST_OBJS) dep.Makefile

.PHONY: depends
depends: dep.Makefile

dep.Makefile: $(TEST_SRCS)
	$(CXX) $(CPPFLAGS) -MM $^ > $@

-include dep.Makefile
