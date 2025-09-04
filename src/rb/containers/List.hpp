#pragma once

#include <initializer_list>

#include <rb/core/assert.hpp>
#include <rb/core/iter/IteratorTraits.hpp>
#include <rb/core/memory/construct.hpp>
#include <rb/core/memory/destroy.hpp>
#include <rb/core/swap.hpp>
#include <rb/ranges/IteratorRange.hpp>

namespace rb::containers {

namespace impl::list {

	struct Node {
		Node* prev;
		Node* next;

		constexpr Node() noexcept
		    : prev(this)
		    , next(this) {
		}
	};

} // namespace impl::list

template <class T>
class List final {
public:
	class ConstIterator final {
		friend class List;

		impl::list::Node const* node_;

		constexpr explicit ConstIterator(impl::list::Node const* node) noexcept
		    : node_(node) {
		}

	public:
		// NOLINTBEGIN(*-identifier-naming)

		using difference_type = isize;
		using iterator_category = std::bidirectional_iterator_tag;
		using pointer = T const*;
		using reference = T const&;
		using value_type = T const;

		// NOLINTEND(*-identifier-naming)

		constexpr T const& operator*() const noexcept {
			return static_cast<Node const*>(node_)->value;
		}

		constexpr T const* operator->() const noexcept {
			return &**this;
		}

		constexpr ConstIterator& operator++() noexcept {
			node_ = node_->next;
			return *this;
		}

		constexpr ConstIterator operator++(int) noexcept {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr ConstIterator& operator--() noexcept {
			node_ = node_->prev;
			return *this;
		}

		constexpr ConstIterator operator--(int) noexcept {
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr bool operator==(ConstIterator const& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		constexpr bool operator!=(ConstIterator const& rhs) const noexcept {
			return node_ != rhs.node_;
		}
	};

	class Iterator final {
		friend class List;

		impl::list::Node* node_;

		constexpr explicit Iterator(impl::list::Node* node) noexcept
		    : node_(node) {
		}

	public:
		// NOLINTBEGIN(*-identifier-naming)

		using difference_type = isize;
		using iterator_category = std::bidirectional_iterator_tag;
		using pointer = T*;
		using reference = T&;
		using value_type = T;

		// NOLINTEND(*-identifier-naming)

		constexpr T& operator*() const noexcept {
			return static_cast<Node*>(node_)->value;
		}

		constexpr T* operator->() const noexcept {
			return &**this;
		}

		constexpr Iterator& operator++() noexcept {
			node_ = node_->next;
			return *this;
		}

		constexpr Iterator operator++(int) noexcept {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr Iterator& operator--() noexcept {
			node_ = node_->prev;
			return *this;
		}

		constexpr Iterator operator--(int) noexcept {
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr bool operator==(Iterator const& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		constexpr bool operator!=(Iterator const& rhs) const noexcept {
			return node_ != rhs.node_;
		}

		// ReSharper disable once CppNonExplicitConversionOperator
		constexpr operator ConstIterator() const noexcept { // NOLINT(*-explicit-constructor)
			return ConstIterator{node_};
		}
	};

	using ConstRange = ranges::IteratorRange<ConstIterator>;
	using Range = ranges::IteratorRange<Iterator>;

	// NOLINTBEGIN(*-identifier-naming)
	using value_type = T;
	using size_type = usize;
	using difference_type = isize;
	using reference = T&;
	using const_reference = T const&;
	using const_iterator = ConstIterator;
	using iterator = Iterator;
	// NOLINTEND(*-identifier-naming)

	// ctor.1
	/// Default constructor.
	/// ~~Constructs an empty container with a default-constructed allocator.~~
	constexpr List() noexcept = default;

	// ctor.3
	/// Constructs the container with @p count copies of @p value.
	List(usize count, T const& value) noexcept(core::isNothrowCopyConstructible<T>) {
		for (; count; --count) {
			pushBack(value);
		}
	}

	// ctor.4
	/// Constructs the container with @p count default-inserted instances of @p T. No copies are made.
	explicit List(usize count) noexcept(core::isNothrowDefaultConstructible<T>) {
		for (; count; --count) {
			emplaceBack();
		}
	}

	// ctor.5
	/// Constructs the container with the contents of the range [@p first, @p last).
	template <class InputIt,
	    RB_REQUIRES_T(core::IsInputIterator<InputIt>)>
	List(InputIt first, InputIt last) {
		for (; first != last; ++first) {
			pushBack(*first);
		}
	}

	// ctor.6
	/// Copy constructor. Constructs the container with the copy of @p rhs.
	List(List const& rhs)
	    : List{rhs.begin(), rhs.end()} {
	}

	// ctor.8
	/// Move constructor. Constructs the container with the contents of @p rhs using move semantics.
	/// ~~Allocator is obtained by move-construction from the allocator belonging to @p rhs.~~
	constexpr List(List&& rhs) noexcept {
		if (rhs.empty()) {
			init();
		} else {
			sentinel_.next = rhs.sentinel_.next;
			sentinel_.prev = rhs.sentinel_.prev;
		}
		rhs.init();
	}

	// ctor.10
	/// Constructs the container with the contents of the initializer list @p il.
	List(std::initializer_list<T> il)
	    : List{il.begin(), il.end()} {
	}

	/// Constructs the container with the contents of the range @p r.
	template <class R,
	    RB_REQUIRES_T(ranges::IsInputRangeNonStrict<R>)>
	List(ranges::FromRange /*fromRange*/, R&& range) {
		for (auto&& r = RB_FWD(range); !ranges::empty(r); ranges::popFront(r)) {
			pushBack(ranges::front(r));
		}
	}

	~List() noexcept(core::isNothrowDestructible<T>) {
		clear();
	}

	/// Copy assignment operator. Replaces the contents with a copy of @p rhs.
	List& operator=(List const& rhs) {
		if (this != &rhs) {
			this->~List();
			new (this) List(rhs);
		}
		return *this;
	}

	/// Move assignment operator.
	/// Replaces the contents with those of @p rhs using move semantics
	/// (i.e., the data in @p rhs is moved from @p rhs into this container).
	/// @p rhs is in a valid but unspecified state afterward.
	List& operator=(List&& rhs) noexcept {
		if (this != &rhs) {
			this->~List();
			new (this) List(RB_MOVE(rhs));
		}
		return *this;
	}

	/// Replaces the contents with those identified by initializer list @p il.
	List& operator=(std::initializer_list<T> il) {
		this->~List();
		new (this) List(il);
		return *this;
	}

	// iteration

	constexpr ConstIterator begin() const noexcept {
		return ConstIterator{sentinel_.next};
	}

	constexpr Iterator begin() noexcept {
		return Iterator{sentinel_.next};
	}

	constexpr ConstIterator end() const noexcept {
		return ConstIterator{&sentinel_};
	}

	constexpr Iterator end() noexcept {
		return Iterator{&sentinel_};
	}

	constexpr ConstIterator cbegin() const noexcept {
		return begin();
	}

	constexpr ConstIterator cend() const noexcept {
		return end();
	}

	constexpr ConstRange range() const noexcept {
		return {begin(), end()};
	}

	constexpr Range range() noexcept {
		return {begin(), end()};
	}

	// empty/size/front/back

	[[nodiscard]] constexpr bool empty() const noexcept {
		return begin() == end();
	}

	constexpr usize size() const noexcept {
		return size_;
	}

	constexpr T const& front() const {
		RB_ASSERT(!empty());
		return *begin();
	}

	constexpr T& front() {
		RB_ASSERT(!empty());
		return *begin();
	}

	constexpr T const& back() const {
		RB_ASSERT(!empty());
		return *--end();
	}

	constexpr T& back() {
		RB_ASSERT(!empty());
		return *--end();
	}

	// modifiers

	/// Erases all elements from the container. After this call, #size returns zero.
	/// Invalidates any references, pointers, and iterators referring to contained elements.
	/// Any past-the-end iterator remains valid.
	void clear() noexcept(core::isNothrowDestructible<T>) {
		auto* node = sentinel_.next;
		while (node != &sentinel_) {
			auto* n = static_cast<Node*>(node);
			node = node->next;
			destroy(n);
		}
		init();
	}

	template <class... Args>
	Iterator emplace(Iterator pos, Args&&... args) noexcept(core::isNothrowConstructible<T, Args...>) {
		auto* node = insertBefore(pos.node_, RB_FWD(args)...);
		return Iterator{node};
	}

	template <class... Args>
	T& emplaceBack(Args&&... args) noexcept(core::isNothrowConstructible<T, Args...>) {
		auto* node = insertBefore(&sentinel_, RB_FWD(args)...);
		return node->value;
	}

	Iterator erase(Iterator pos) noexcept(core::isNothrowDestructible<T>) {
		if (pos == end()) {
			return pos;
		}

		auto* next = pos.node_->next;
		auto* prev = pos.node_->prev;
		prev->next = next;
		next->prev = prev;

		auto* node = static_cast<Node*>(pos.node_);
		destroy(node);
		return Iterator{next};
	}

	Iterator erase(Iterator first, Iterator last) noexcept(core::isNothrowDestructible<T>) {
		while (first != last) {
			first = erase(first);
		}
		return last;
	}

	Iterator insert(Iterator pos, T const& value) noexcept(core::isNothrowCopyConstructible<T>) {
		auto* node = insertBefore(pos.node_, value);
		return Iterator{node};
	}

	Iterator insert(Iterator pos, T&& value) noexcept(core::isNothrowMoveConstructible<T>) {
		auto* node = insertBefore(pos.node_, RB_MOVE(value));
		return Iterator{node};
	}

	Iterator insert(Iterator pos, usize count, T const& value) {
		if (!count) {
			return pos;
		}

		List list(count, value);
		auto it = list.begin();
		splice(pos, list);
		return it;
	}

	template <class InputIt,
	    RB_REQUIRES_T(core::IsInputIterator<InputIt>)>
	Iterator insert(Iterator pos, InputIt first, InputIt last) {
		List list(first, last);
		if (list.empty()) {
			return pos;
		}

		auto it = list.begin();
		splice(pos, list);
		return it;
	}

	Iterator insert(Iterator pos, std::initializer_list<T> il) {
		return insert(pos, il.begin(), il.end());
	}

	void popBack() noexcept(core::isNothrowDestructible<T>) {
		erase(--end());
	}

	void popFront() noexcept(core::isNothrowDestructible<T>) {
		erase(begin());
	}

	void pushBack(T const& value) noexcept(core::isNothrowCopyConstructible<T>) {
		insert(end(), value);
	}

	void pushBack(T&& value) noexcept(core::isNothrowMoveConstructible<T>) {
		insert(end(), RB_MOVE(value));
	}

	void pushFront(T const& value) noexcept(core::isNothrowCopyConstructible<T>) {
		insert(begin(), value);
	}

	void pushFront(T&& value) noexcept(core::isNothrowMoveConstructible<T>) {
		insert(begin(), RB_MOVE(value));
	}

	usize remove(T const& value) {
		usize count = 0;
		auto first = begin();
		auto last = end();
		while (first != last) {
			if (*first == value) {
				first = erase(first);
				++count;
			} else {
				++first;
			}
		}
		return count;
	}

	template <class UnaryPredicate>
	usize removeIf(UnaryPredicate pred) {
		usize count = 0;
		auto first = begin();
		auto last = end();
		while (first != last) {
			if (pred(*first)) {
				first = erase(first);
				++count;
			} else {
				++first;
			}
		}
		return count;
	}

	void resize(usize count) {
		if (count == size_) {
			return;
		}

		if (count > size_) {
			for (count -= size_; count > 0; --count) {
				emplaceBack();
			}
			return;
		}

		auto it = begin();
		std::advance(it, count);
		erase(it, end());
	}

	void resize(usize count, T const& value) {
		if (count == size_) {
			return;
		}

		if (count > size_) {
			insert(end(), count - size_, value);
			return;
		}

		auto it = begin();
		std::advance(it, count);
		erase(it, end());
	}

	void splice(Iterator pos, List& list) noexcept {
		splice(pos, RB_MOVE(list));
	}

	void splice(Iterator pos, List&& list) noexcept {
		// there is UB if this == &list
		if (this == &list || list.empty()) {
			return;
		}

		transfer(pos, list.begin(), list.end());
		size_ += list.size_;
		list.size_ = 0;
	}

	constexpr void swap(List& rhs) noexcept {
		if (empty() || rhs.empty()) {
			auto& other = empty() ? *this : rhs;
			other.sentinel_.next = sentinel_.next;
			other.sentinel_.prev = sentinel_.prev;
			other.sentinel_.next->prev = &other.sentinel_;
			other.sentinel_.prev->next = &other.sentinel_;
			init();
		} else {
			core::swap(sentinel_, rhs.sentinel_);
			core::swap(sentinel_.prev->next, rhs.sentinel_.prev->next);
			core::swap(sentinel_.next->prev, rhs.sentinel_.next->prev);
		}
	}

	usize unique() {
		return unique([](T const& lhs, T const& rhs) {
			return lhs == rhs;
		});
	}

	template <class BinaryPredicate>
	usize unique(BinaryPredicate pred) {
		if (size_ < 2) {
			return 0;
		}

		usize count = 0;
		auto first = begin();
		auto const last = end();
		auto next = first;
		while (++next != last) {
			if (pred(*first, *next)) {
				erase(next);
				next = first;
				++count;
			} else {
				first = next;
			}
		}
		return count;
	}

private:
	struct Node : impl::list::Node {
		T value;
	};

	// Moves the elements from [first, last) before pos.
	static constexpr void transfer(impl::list::Node* pos, impl::list::Node* first, impl::list::Node* last) noexcept {
		if (first == last) {
			return;
		}

		// remove [first, last) from its old position
		last->prev->next = pos;
		first->prev->next = last;
		pos->prev->next = first;
		// splice [first, last) into its new position
		auto* prev = pos->prev;
		pos->prev = last->prev;
		last->prev = first->prev;
		first->prev = prev;
	}

	static constexpr void transfer(iterator pos, iterator first, iterator last) noexcept {
		transfer(pos.node_, first.node_, last.node_);
	}

	template <class... Args>
	Node* construct(Args&&... args) noexcept(core::isNothrowConstructible<T, Args...>) {
		auto* node = static_cast<Node*>(::operator new(sizeof(Node)));
		try {
			auto* ptr = core::addressOf(node->value);
			core::construct(ptr, RB_FWD(args)...);
		} catch (...) {
			::operator delete(node);
			throw;
		}
		return node;
	}

	void destroy(Node* node) noexcept(core::isNothrowDestructible<T>) {
		core::destroy(core::addressOf(node->value));
		::operator delete(node);
		--size_;
	}

	constexpr void init() noexcept {
		sentinel_.next = &sentinel_;
		sentinel_.prev = &sentinel_;
	}

	template <class... Args>
	Node* insertBefore(impl::list::Node* pos, Args&&... args) noexcept(core::isNothrowConstructible<T, Args...>) {
		auto* node = construct(RB_FWD(args)...);
		node->next = pos;
		node->prev = pos->prev;
		pos->prev->next = node;
		pos->prev = node;
		++size_;
		return node;
	}

	impl::list::Node sentinel_;
	usize size_ = 0;
};

template <class InputIt,
    RB_REQUIRES_T(core::IsInputIterator<InputIt>)>
List(InputIt first, InputIt last) -> List<typename core::IteratorTraits<InputIt>::Value>;

template <class R,
    RB_REQUIRES_T(ranges::IsInputRangeNonStrict<core::RemoveRef<R>>)>
List(ranges::FromRange, R&& range) -> List<core::RemoveRef<ranges::ValueType<core::RemoveRef<R>>>>;

template <class T>
constexpr bool operator==(List<T> const& lhs, List<T> const& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	auto first = lhs.begin();
	auto const last = lhs.end();
	auto second = rhs.begin();
	while (first != last) {
		if (*first++ != *second++) {
			return false;
		}
	}
	return true;
}

template <class T>
constexpr bool operator!=(List<T> const& lhs, List<T> const& rhs) {
	return !(lhs == rhs);
}

template <class T, class U = T>
usize erase(List<T>& list, U const& value) {
	return list.removeIf([&](auto& x) { return x == value; });
}

template <class T, class UnaryPredicate>
usize eraseIf(List<T>& list, UnaryPredicate pred) {
	return list.removeIf(pred);
}

} // namespace rb::containers
