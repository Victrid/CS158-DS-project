# vector

This version of [vector](https://github.com/Victrid/CS158-DS-project/blob/master/vector/vector.hpp) is well commented. You can read the code with well-guided instructions.

## Implementation

### Data storage part

```
+--------------------+
|                    |
|       vector       |    +---------+
|                    | +->+  Data   |
| +----------------+ | |  +---------+
| |     Size       | | |  |  Data   |
| +----------------+ | |  +---------+
|                    | |  |  Data   |
| +----------------+ | |  +---------+
| |    Capacity    | | |  |  .      |
| +----------------+ | |  |  .      |
|                    | |  |  .      |
|  +---------------+ | |  |         |
|  |*| container   +---+  +---------+
|  +---------------+ |    |FULL FLAG|
|                    |    +---------+
+--------------------+
```

The last one in the list is used as a flag to mark if the container is full.

When the container is full, it will be copied to a bigger list.

When `std::move` is not allowed in this environment, we have the ancient C++ way to deep-copy them, which requires the datum has a ctor `T(const T&)`.

To avoid extra time consuming of space initiation, we use [`operator new`](https://blog.victrid.dev/2020/03/31/nt-new/) to store them in the continuous space.

### Iteration part

```
+------------------------------+
|  vector    <---------------+ |
|                            | |
|  +----------------------+  | |
|  |    iterator          |  | |
|  | +------------------+ |  | |
|  | |*|        v       +----+ |
|  | +------------------+ |    |
|  |                      |    |
|  | +------------------+ |    |
|  | | Delta            | |    |
|  | +------------------+ |    |
|  |                      |    |
|  | +------------+       |    |
|  | |Legal flag  |       |    |
|  | +------------+       |    |
|  |                      |    |
|  +----------------------+    |
+------------------------------+
```

Very easy iterator. Delta refers to v[Delta], and the Legal flag checking if the position is legal.

Iterator are not strictly binded to the vector, so functions using them depends on checks to the vector itself.

