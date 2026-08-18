CXX      ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Iinclude

SRC := src/stats.cpp src/main.cpp
BIN := stats

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	echo "1 2 3 4 5 6 7 8 9 10" | ./$(BIN)

TEST_SRC := $(wildcard tests/test_*.cpp)
TEST_BIN := $(patsubst tests/%.cpp,tests/%,$(TEST_SRC))

tests/%: tests/%.cpp src/stats.cpp include/stats.hpp tests/test_util.hpp
	$(CXX) $(CXXFLAGS) -Itests $< src/stats.cpp -o $@

test: $(TEST_BIN)
	@fail=0; for t in $(TEST_BIN); do ./$$t || fail=1; done; exit $$fail

clean:
	rm -f $(BIN) $(TEST_BIN)

.PHONY: run test clean
