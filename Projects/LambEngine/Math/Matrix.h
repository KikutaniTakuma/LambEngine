#pragma once
#include <array>
#include <type_traits>

template<std::floating_point floatingType, size_t height_, size_t width_>
class Matrix {
public:
	using value_type = floatingType;

	using width_type = std::array<value_type, width_>;
	using height_type = std::array<width_type, height_>;

	using vector_type = std::array<value_type, height_* width_>;
	using matrix_type = height_type;


public:
	Matrix()noexcept :
		data()
	{}

	Matrix(const vector_type& num) noexcept {
		for (size_t count = 0; width_type & line : data) {
			for (value_type& i : line) {
				i = num[count];
				count++;
			}
		}
	}
	Matrix(const Matrix&) = default;
	Matrix(Matrix&&) = default;
	virtual ~Matrix() = default;

public:
	Matrix& operator=(const Matrix&) = default;
	Matrix& operator=(Matrix&&) = default;

public:
	template<std::integral T>
	width_type& operator[](T index) noexcept {
		return data[index];
	}

	template<std::integral T>
	const width_type& operator[](T index) const noexcept {
		return data[index];
	}

public:
	template<size_t otherWidth>
	Matrix<value_type, height_, otherWidth> operator*(const Matrix<value_type, width_, otherWidth>& right) const noexcept {
		Matrix<value_type, height_, otherWidth> result;

		for (size_t y = 0; y < height_; y++) {
			for (size_t x = 0; x < otherWidth; x++) {
				for (size_t i = 0; i < width_; i++) {
					result[y][x] += data[y][i] * right[i][x];
				}
			}
		}

		return result;
	}
	template<size_t otherWidth>
	Matrix<value_type, height_, otherWidth>& operator*=(const Matrix<value_type, height_, otherWidth>& right)noexcept {
		*this = *this * right;

		return *this;
	}

	Matrix operator+(const Matrix& right) const noexcept {
		Matrix result;

		for (size_t y = 0; y < height_; y++) {
			for (size_t x = 0; x < width_; x++) {
				result[y][x] = data[y][x] + right[y][x];
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

		for (size_t y = 0; y < height_; y++) {
			for (size_t x = 0; x < width_; x++) {
				result[y][x] = data[y][x] - right[y][x];
			}
		}

		return result;
	}
	Matrix& operator-=(const Matrix& right)noexcept {
		*this = *this - right;

		return *this;
	}

public:
	constexpr size_t height_size() const noexcept {
		return height_;
	}

	constexpr size_t width_size() const noexcept {
		return width_;
	}

	value_type* data() const noexcept {
		return reinterpret_cast<value_type*>(this);
	}

protected:
	matrix_type data;
};