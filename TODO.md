# TODO

## `C++`

### `algo`

- [ ] `constexpr` algorithms
  - [ ] `all`/`any`
  - [ ] `find`/`find_if`
  - [ ] `count`/`count_if`

### `containers`

- [ ] `Deque`
- [ ] `List`
- [ ] `Map`
- [ ] `PriorityQueue`
- [ ] `Set`
- [ ] `Stack`
- [ ] `Queue`
- [ ] `Vector`
  - [ ] construct from range efficiently

### `core`

- [ ] `opCmp`
- [ ] `reflect`
  - [ ] enums
  - [ ] `TypeInfo`
    - [ ] static
    - [ ] dynamic
    - [ ] demangle
- [ ] `OsError`: introduce mapping from real system error codes
- [ ] `Error`
  - [ ] immutable string
  - [ ] ref-counted pointer instead of `std::shared_ptr`
- [ ] `Optional` like in C++23
  - [ ] `Option<T&&>`?
- [ ] `Variant`
- [ ] type of container (random-access, associative, tuple-like)
- [ ] `TypeList` and `ArgList` with `auto` template parameters
- [ ] `constexpr` in C++20
  - [ ] `UniquePtr`
- [ ] concepts
- [x] `i128`/`u128`
  - [ ] floating-point ops: `*`, `/`
  - [ ] conversions from/to string
- [ ] `quorem`
  - [ ] use single asm instruction
  - [ ] look at `<cmath>` for behavior details
- [ ] `QFlag`
  - [ ] `BitMask` trait
- [ ] `CompressedPair` for both template args
- [ ] own string class
- [ ] allocator-aware library wrappers for dynamic allocation
- [ ] `AllocUniquePtr`/`AllocOwnerPtr`
- [ ] strong typedef (or `newtype`) idiom
- [ ] `MallocDeleter` and `ArrayAllocator` for `malloc`ed arrays
- [ ] `boost::operators`
- [ ] `StaticArray` as analogue of `StaticString` for arrays (i.e. sliceable `std::array`)
- [ ] fixed width floating-point types (see [link](https://en.cppreference.com/w/cpp/types/floating-point))
  - [ ] `f16`
  - [ ] `f128`
  - [ ] `bf16`
  - [ ] literal suffixes
  - [ ] type traits
- [ ] Clang's nullability attributes for types
- [ ] exception spec for expressions `new T(args...)` (infer `noexcept` if&nbsp;`std::bad_alloc` is the only one)
  - [ ] `UniquePtr`
  - [ ] `OwnerPtr`
  - [ ] `Any`
- [x] `Any`
  - [ ] small-object optimization
  - [ ] visitor (see [std::any::type](https://en.cppreference.com/w/cpp/utility/any/type))
  - [ ] `constexpr`
  - [ ] disabled RTTI
- [ ] `memory`
  - [ ] full impl of `Allocator`
  - [ ] revise requirements to `UniquePtr` (see `operator*`, `operator->` and so on)
- [ ] `core::error`: POSIX error codes are implementation-dependent
- [ ] `std::is_constant_evaluated` (see `memory::construct`)
- [ ] C++20's `IteratorTraits`
- [x] `Expected`
  - [ ] revise `RB_NOEXCEPT_LIKE`
  - [ ] use `Variant`
  - [ ] reference support?
- [ ] SAL annotations
- [ ] make `countLeadingZeroes` `constexpr` on MSVC
- [ ] `RB_DEFINE_FLAG`?

### Data structures

- [ ] hash table
- [ ] heap
- [ ] tree
  - [ ] AVL tree
  - [ ] B-tree
  - [ ] red-black tree

### Data exchange formats

#### `json`

#### `xml`

#### `ini`

### `fmt`

- [ ] pretty printing
  - [ ] [AbslStringify](https://abseil.io/docs/cpp/guides/abslstringify)
- [ ] debug printing a-la Rust or Python
- [ ] autodetect container type (tuple-like, random access, associative)
- [ ] scoped enums
- [ ] fix `iterables.hpp` (`<<` is very eager)
- [ ] `charconv` for floating-point
- [ ] `quoted` for `char const*` (compare with `std::quoted`)
- [ ] [Abseil strings library](https://abseil.io/docs/cpp/guides/strings)

### `fs`

- [ ] `OsString`
- [ ] `Path`

### `io`

- [ ] `InputStream`
- [ ] `QIODevice` analogue?
- [ ] `OutputStream`
- [ ] use `Option<char>` instead of `int` for `char_traits::eof`
- [ ] ranges for `Readable` and `Writable`

### `log`

### `math`

- [ ] `constexpr` versions of functions in `<cmath>`
  - [ ] `frexp`
  - [ ] `ldexp`
- [ ] `constexpr` version of `std::bitset`

### `msg`

- [ ] `ObserverList`
- [ ] `Message`
  - [ ] `std::any`
  - [ ] like `QEvent`
  - [ ] `TypedMessage<Args...>`
- [ ] `SyncQueue`
- [ ] `Thread`
  - [ ] message loop
  - [ ] handle return type and exceptions
- [ ] events
- [ ] signals
- [ ] timers

### `proto::dcp2`

- [ ] parser
- [ ] server
  - [ ] nets
  - [ ] net manager
  - [ ] devices
  - [ ] properties
  - [ ] requests
  - [ ] virtual devices
  - [ ] packet scheduler

### `random`

- [ ] correct SFINAE in random traits
- [ ] `uniform` with different types of ends
- [ ] `Uniform`
- [ ] `BitGen` as a range
- [ ] add open-close interval type to `uniform`

### `ranges`

- [x] traits
  - [ ] `IsOutputRangeImpl` for `const R`
- [ ] `Take`
- [ ] `Generator`
- [ ] algorithms
  - [ ] `find`
  - [ ] `min`/`max`
  - [ ] `fill`
  - [ ] `count`, `count_if`
  - [ ] `enumerate`
  - [ ] `generate`
  - [ ] `all`/`any`
- [ ] `startsWith`/`endsWith`
- [ ] char/string conversions like in D
- [ ] `std.range.interfaces`
- [ ] conversion to a pair of iterators

### `stacktrace`

### `sync`

- [ ] adopted threads
- [ ] thread name
- [ ] `rb_once_flag` static init
- [ ] implement `StdThread` using `OsThread`
- [ ] `std::jthread`
- [ ] [test for mutex](https://demin.ws/blog/russian/2009/02/06/universal-mutex-in-cpp-for-windows-and-unix/)
- [ ] impl `once` via pimpl?
- [ ] make `Thread::start` `noexcept`
- [ ] Windows
  - [ ] condition variable wait for/until
  - [ ] non-recursive mutex
  - [ ] spinlock
  - [ ] timed mutex

### `thread`

- [ ] move to `msg`? or introduce `os`?
- [ ] fibers
  - [ ] fiber/thread scheduler like in D?
- [ ] `ThreadGroup`

### `time`

- [ ] conversions in `Duration`
- [ ] `SysTime`, `MonoTime` or `SysClock`, `MonoClock`
- [ ] `HighResolutionClock`
- [ ] time(rs) and testing
- [ ] `std::time` in `Rust`: `Instant`, `SystemTime`

### `utf`

- [ ] `Utf8String`, `Utf16String`, `Utf32String`
- [ ] string conversions

## `test`

- [ ] `rapidcheck`

## Unsorted

- [ ] processes
- [ ] `getopt`
  - [ ] see Abseil flags
- [ ] custom floating-point (see `CustomFloat` in D)
- [ ] big integers
- [ ] graphs
- [ ] hash functions
  - [ ] CRC
  - [ ] MD5
  - [ ] MurmurHash
  - [ ] SHA
- [ ] zip
- [ ] CSV
- [ ] Base64
- [ ] environment
- [ ] sockets
- [ ] regular expressions
- [ ] quaternions
