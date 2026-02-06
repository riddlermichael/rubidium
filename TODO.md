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

- [ ] `opCmp` (see [std::cmp_equal](https://en.cppreference.com/w/cpp/utility/intcmp.html))
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
  - [ ] `cause`
- [ ] `Option` like in C++26
  - [ ] `Option<void>`
  - [ ] `Option<T&>` and `Option<T&&>`?
- [ ] `Variant`
- [ ] `Tuple`
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
  - [ ] check [constructors](https://t.ly/vzrcn)
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
  - [ ] `absl::Any`
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
- [ ] `max` -> `kMax`
- [ ] `UniquePtr<byte>`
- [ ] `core::str`: ascii functions (see `absl/strings/ascii.h`)
- [ ] look at Chromium's `base::Location`
- [ ] compare `Array` and `absl::FixedArray`
- [ ] see `absl::Nullable` etc.
  - [ ] Rust: `ptr::NonNull`, `num::NonZero`
  - [ ] should work in case of `struct { NonZero field; }`
- [ ] `noexcept` detection
- [ ] `czstring`: `__null_terminated` support
- [ ] range checking in literals `_i8` etc.
- [ ] `SourceLocation` with compile-time strings
- [ ] [safe](https://t.ly/W6-H9) `min`
- [ ] `std::function` ([see](https://t.ly/OqKdm))
- [ ] Iterators like in Rust
- [ ] [Dreams](https://habr.com/ru/articles/330402) about `std::of`
- [ ] [SharedPtr](https://t.ly/Un-7M)

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
- [ ] `StreamStateSaver`: use trait for `Stream`
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

- [ ] math constants ([std](https://en.cppreference.com/w/cpp/header/numbers.html),
  [boost](https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/constants.html))
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

- [ ] look at Chromium's `base::RandomBitGenerator`, `base/rand_util.h`
  for a cryptographically secure generator
- [ ] correct SFINAE in random traits
- [ ] `uniform` with different types of ends
- [ ] `Uniform`
- [ ] `BitGen` as a range
- [ ] add open-close interval type to `uniform`
- [ ] [uniform](https://t.ly/Stl6W) distribution for `std::byte`/`core::byte`

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
- [ ] `std::jthread`
- [ ] [test for mutex](https://demin.ws/blog/russian/2009/02/06/universal-mutex-in-cpp-for-windows-and-unix/)
- [ ] `once`
  - [ ] static init
- [ ] make `Thread::start` `noexcept` (catch exceptions)
- [ ] Windows
  - [ ] condition variable wait for/until
  - [ ] non-recursive mutex
  - [ ] spinlock
  - [ ] timed mutex
- [ ] thread stack size
- [ ] thread timed join (i.e., wait)
- [ ] `Thread::spawn` (like in `std`)

### `thread`

- [ ] move to `msg`? or introduce `os`?
- [ ] fibers
  - [ ] fiber/thread scheduler like in D?
- [ ] `ThreadGroup`

### `time`

- [x] use `absl::Duration`; move `i128` representation to `LongDuration`
- [x] look at Rust's `std::time`: `Duration`, `Instant`, `SystemTime`
- [x] conversions in `Duration`
- [x] `SysTime`, `MonoTime` or `SysClock`, `MonoClock`
- [x] ~~`HighResolutionClock`~~
- [ ] time(rs) and testing
- [ ] `Nanosecond`, `Microsecond`, `Millisecond`, `Second`, `Minute`, `Hour`, `Day` types
- [ ] clock resolution
- [ ] macOS: `kMonotonicFast`, `kRealtimeFast`, `kUptime`
- [ ] `DURATION_NAN` feature: why only for `LongDuration`?

### `utf`

- [ ] `Utf8String`, `Utf16String`, `Utf32String`
- [ ] string conversions

## `test`

- [ ] `rapidcheck`

## Unsorted

- [ ] disable exceptions:
  - GCC/Clang: `-fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables`
  - MSVC: https://share.google/aimode/p548iQFwXTMTfjyvQ
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
- [ ] `RB_DEBUG`
- [ ] Jeff Alger ([see](https://t.ly/yOAXW))
- [ ] `cmake_host_system_information`
