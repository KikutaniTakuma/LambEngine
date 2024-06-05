#pragma once
#include <array>
#include "Error/Error.h"

#include "Concepts.h"

namespace Lamb {
    template<Lamb::IsObject Type, size_t kSize> requires(0 < kSize)
        class Array {
        static_assert(0 < kSize, "Size must be greater than 0");
        static_assert(std::is_object_v<Type>, "non-object types");

        public:
            using value_type = Type;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
            using pointer = Type*;
            using const_pointer = const Type*;
            using reference = Type&;
            using const_reference = const Type&;

            using iterator = std::_Array_iterator<Type, kSize>;
            using const_iterator = std::_Array_const_iterator<Type, kSize>;

            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        public:
            constexpr void fill(const Type& num) {
                std::fill_n(element_, kSize, num);
            }

            constexpr void swap(Array& _Other) noexcept(std::_Is_nothrow_swappable<Type>::value) {
                std::_Swap_ranges_unchecked(element_, element_ + kSize, _Other.element_);
            }

            [[nodiscard]] constexpr iterator begin() noexcept {
                return iterator(element_, 0);
            }

            [[nodiscard]] constexpr const_iterator begin() const noexcept {
                return const_iterator(element_, 0);
            }

            [[nodiscard]] constexpr iterator end() noexcept {
                return iterator(element_, kSize);
            }

            [[nodiscard]] constexpr const_iterator end() const noexcept {
                return const_iterator(element_, kSize);
            }

            [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
                return reverse_iterator(end());
            }

            [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
                return const_reverse_iterator(end());
            }

            [[nodiscard]] constexpr reverse_iterator rend() noexcept {
                return reverse_iterator(begin());
            }

            [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
                return const_reverse_iterator(begin());
            }

            [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
                return begin();
            }

            [[nodiscard]] constexpr const_iterator cend() const noexcept {
                return end();
            }

            [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
                return rbegin();
            }

            [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
                return rend();
            }

            [[nodiscard]] constexpr size_type size() const noexcept {
                return kSize;
            }

            [[nodiscard]] constexpr size_type max_size() const noexcept {
                return kSize;
            }

            [[nodiscard]] constexpr bool empty() const noexcept {
                return false;
            }

            [[nodiscard]] constexpr reference at(size_type index) {
                if (kSize <= index) [[unlikely]] {
                    OutOfRange(index);
                    }

                return element_[index];
            }

            [[nodiscard]] constexpr const_reference at(size_type index) const {
                if (kSize <= index) [[unlikely]] {
                    OutOfRange(index);
                    }

                return element_[index];
            }

            [[nodiscard]] constexpr reference operator[](size_type index) noexcept {
                if (kSize <= index) [[unlikely]] {
                    OutOfRange(index);
                    }
                return element_[index];
            }

            [[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept {
                if (kSize <= index) [[unlikely]] {
                    OutOfRange(index);
                    }
                return element_[index];
            }

            [[nodiscard]] constexpr reference front() noexcept {
                return element_[0];
            }

            [[nodiscard]] constexpr const_reference front() const noexcept {
                return element_[0];
            }

            [[nodiscard]] constexpr reference back() noexcept {
                return element_[kSize - 1];
            }

            [[nodiscard]] constexpr const_reference back() const noexcept {
                return element_[kSize - 1];
            }

            [[nodiscard]] constexpr Type* data() noexcept {
                return element_;
            }

            [[nodiscard]] constexpr const Type* data() const noexcept {
                return element_;
            }

            value_type element_[kSize];

        private:
            static void OutOfRange(size_t index) {
                throw Lamb::Error::Code<Array<Type, kSize>>("out of range -> index is " + std::to_string(index), ErrorPlace);
            }

    };
    
    // 推論補助
#if _HAS_CXX17
    template <class First, class... Rest>
    struct Enforce_same {
        static_assert(
            std::conjunction_v<std::is_same<First, Rest>...>, "N4950 [array.cons]/2: Mandates: (is_same_v<T, U> && ...) is true.");
        using type = First;
    };

    template <class First, class... Rest>
    Array(First, Rest...) -> Array<typename Enforce_same<First, Rest...>::type, 1 + sizeof...(Rest)>;
#endif // _HAS_CXX17

    template<typename Type, size_t kSize>
    [[nodiscard]] constexpr bool operator==(const Array<Type, kSize>& left, const Array<Type, kSize>&right) {
        return std::equal(left.data(), left.data() + kSize, right.data());
    }

#if !_HAS_CXX20
    template<typename Type, size_t kSize>
    [[nodiscard]] constexpr bool operator==(const Array<Type, kSize>& left, const Array<Type, kSize>& right) {
        return not (left == right);
    }
#endif // !_HAS_CXX20

#ifdef __cpp_lib_concepts
   template <class Type, size_t kSize>
        [[nodiscard]] constexpr std::_Synth_three_way_result<Type> operator<=>(
            const Array<Type, kSize>& left, const Array<Type, kSize>& right) {
        return std::lexicographical_compare_three_way(
            left.data(), left.data() + kSize, right.data(), right.data() + kSize, std::_Synth_three_way{});
   }
#else
    template <class Type, size_t kSize>
    [[nodiscard]] _CONSTEXPR20 bool operator<(const array<Type, kSize>& left, const array<Type, kSize>& right) {
        return _STD lexicographical_compare(left.data(), left.data() + kSize, right.data(), right.data() + kSize);
    }

    template <class Type, size_t kSize>
    [[nodiscard]] _CONSTEXPR20 bool operator>(const array<Type, kSize>& left, const array<Type, kSize>& right) {
        return right < left;
    }

    template <class Type, size_t kSize>
    [[nodiscard]] _CONSTEXPR20 bool operator<=(const array<Type, kSize>& left, const array<Type, kSize>& right) {
        return !(right < left);
    }

    template <class Type, size_t kSize>
    [[nodiscard]] _CONSTEXPR20 bool operator>=(const array<Type, kSize>& left, const array<Type, kSize>& right) {
        return !(left < right);
    }
#endif
}