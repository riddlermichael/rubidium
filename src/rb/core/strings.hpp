#pragma once

namespace rb::core {

/// LiteralBasicString is a pointer to a literal (i.e. read-only static storage) C-string (zero-terminated array)
/// with a templated @p Char type.
template <class Char>
using LiteralBasicString = Char const*;

/// Literal zero terminated char string.
using LiteralString = LiteralBasicString<char>;

} // namespace rb::core
