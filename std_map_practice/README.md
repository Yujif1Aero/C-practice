# `std::map` Practice

`std::map<Key, Value>` is an ordered dictionary-like container in C++.

It stores pairs:

```text
key -> value
```

Example:

```cpp
std::map<std::string, int> age;
age["Lisa"] = 24;
age["Anna"] = 31;
```

This means:

```text
"Lisa" -> 24
"Anna" -> 31
```

Important properties:

- each key is unique
- keys are automatically sorted
- lookup, insert, and erase are usually `O(log n)`
- `map[key]` creates the key if it does not exist
- `map.at(key)` reads an existing key and throws if missing

## Build

```bash
make
```

## Run

```bash
./map_basics
./word_count
./map_vs_vector
```

## Pure Virtual Practice

`pure_virtual_basics.cpp` shows an abstract base class:

```cpp
virtual void run() = 0;
```

That means `ResearchTask` defines an interface, but cannot be used directly.
A derived class such as `SimulationTask` must implement `run()`.

`virtual_destructor_demo.cpp` shows the correct pattern:

```cpp
virtual ~ContextBase() = default;
```

Use this when deleting a derived object through a base pointer, for example:

```cpp
std::unique_ptr<ContextBase> context = std::make_unique<RefinementContext>();
```

`non_virtual_destructor_warning.cpp` intentionally demonstrates the bad pattern.
Compile it with `clang++` and you should see why a polymorphic base class needs a virtual destructor.
