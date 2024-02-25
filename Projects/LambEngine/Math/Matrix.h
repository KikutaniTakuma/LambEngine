#pragma once
#include <array>
#include <type_traits>

#include "Error/Error.h"

template<std::floating_point floatingType, size_t height, size_t width = height>
class Matrix {
public:
	using ValueType = floatingType;

	using WidthType = std::array<ValueType, width>;
	using HeightType = std::array<WidthType, height>;

	using VectorType = std::array<ValueType, height * width>;
	using MatrixType = HeightType;

	using iterator = MatrixType::iterator;
	using const_iterator = MatrixType::const_iterator;

	using reverse_iterator = MatrixType::reverse_iterator;
	using const_reverse_iterator = MatrixType::const_reverse_iterator;

public:
	constexpr Matrix() = default;

	constexpr Matrix(const VectorType& num) noexcept {
		for (size_t count = 0; WidthType & line : matirx_) {
			for (ValueType& i : line) {
				i = num[count];
				count++;
			}
		}
	}
	constexpr Matrix(const Matrix&) = default;
	constexpr Matrix(Matrix&&) = default;
	virtual ~Matrix() = default;

public:
	Matrix& operator=(const Matrix&) = default;
	Matrix& operator=(Matrix&&) = default;

public:
	template<std::integral T>
	[[nodiscard]] constexpr WidthType& operator[](T index) {
		if (matirx_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}

		return matirx_[index];
	}

	template<std::integral T>
	[[nodiscard]] constexpr const WidthType& operator[](T index) const {
		if (matirx_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}
		return matirx_[index];
	}

public:
	template<size_t otherWidth>
	Matrix<ValueType, height, otherWidth> operator*(const Matrix<ValueType, width, otherWidth>& right) const noexcept {
		Matrix<ValueType, height, otherWidth> result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < otherWidth; x++) {
				for (size_t i = 0; i < width; i++) {
					result[y][x] += matirx_[y][i] * right[i][x];
				}
			}
		}

		return result;
	}
	template<size_t otherWidth>
	Matrix<ValueType, height, otherWidth>& operator*=(const Matrix<ValueType, height, otherWidth>& right)noexcept {
		*this = *this * right;

		return *this;
	}

	Matrix operator+(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matirx_[y][x] + right[y][x];
			}
		}

		return result;
	}
	Matrix& operator+=(const Matrix& right)noexcept {
		*this = *this + right;

		return *this;
	}
	Matrix operator-(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				result[y][x] = matirx_[y][x] - right[y][x];
			}
		}

		return result;
	}
	Matrix& operator-=(const Matrix& right)noexcept {
		*this = *this - right;

		return *this;
	}

public:
	bool operator==(const Matrix& right) const {
		return vector_ == right.vector_;
	}
	bool operator!=(const Matrix& right) const {
		return vector_ != right.vector_;
	}

public:
	[[nodiscard]] constexpr size_t HeightSize() const noexcept {
		return height;
	}

	[[nodiscard]] constexpr size_t WidthSize() const noexcept {
		return width;
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return height * width;
	}

	[[nodiscard]] constexpr iterator begin() noexcept {
		return matirx_.begin();
	}
	[[nodiscard]] constexpr const_iterator begin() const noexcept {
		return matirx_.begin();
	}

	[[nodiscard]] constexpr iterator end() noexcept {
		return matirx_.end();
	}
	[[nodiscard]] constexpr const_iterator end() const noexcept {
		return matirx_.end();
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
		return matirx_.rbegin();
	}
	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
		return matirx_.rbegin();
	}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept {
		return matirx_.rend();
	}
	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
		return matirx_.rend();
	}

	[[nodiscard]] constexpr ValueType& at(size_t index) {
		if (vector_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}

		return vector_.at(index);
	}

	[[nodiscard]] constexpr const ValueType& at(size_t index) const {
		if (vector_.size() <= index) {
			throw Lamb::Error::Code<Matrix>("out of range", __func__);
		}
		return vector_.at(index);
	}

	[[nodiscard]] ValueType* data() noexcept {
		return reinterpret_cast<ValueType*>(this);
	}

	[[nodiscard]] const ValueType* data() const noexcept {
		return reinterpret_cast<const ValueType*>(this);
	}

	[[nodiscard]] constexpr VectorType& view() noexcept {
		return vector_;
	}
	[[nodiscard]] constexpr const VectorType& view() const noexcept {
		return vector_;
	}

	[[noreturn]] constexpr void swap(Matrix& right) {
		vector_.swap(right.vector_);
	}


/// <summary>
/// 正方行列のみ
/// </summary>
public:
	static const Matrix& Identity() requires (height == width)
	{
		static Matrix identity;

		for (size_t i = 0; i < height; i++) {
			identity[i][i] = 1.0f;
		}

		return identity;
	}

protected:
	union {
		MatrixType matirx_;
		VectorType vector_;
	};
};