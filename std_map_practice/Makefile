CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -O0 -g

TARGETS := map_basics word_count map_vs_vector find_vs_brackets pair_vs_map

all: $(TARGETS)

map_basics: map_basics.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

word_count: word_count.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

map_vs_vector: map_vs_vector.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

find_vs_brackets: find_vs_brackets.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

pair_vs_map: pair_vs_map.cpp
	$(CXX) $(CXXFLAGS) $< -o $@


clean:
	rm -f $(TARGETS)
