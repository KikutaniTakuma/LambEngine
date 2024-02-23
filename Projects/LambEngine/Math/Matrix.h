#pragma once
#include <array>
#include <type_traits>

template<size_t Y, size_t X, std::floating_point floatingType>
class Matrix {
public:
	using type = floatingType;
	using height = Y;
	using width = X;

	using WidthType = std::array<type, width>;
	using HeightType = std::array<WidthType, height>;


public:
	Matrix() = default;
	Matrix(const Matrix&) = default;
	Matrix(Matrix&&) = default;
	~Matrix() = default;

public:
	Matrix& operator=(const Matrix&) = default;
	Matrix& operator=(Matrix&&) = default;

public:
	template<std::integral T>
	WidthType& operator[](T index) {
		return data_[index];
	}

	template<std::integral T>
	const WidthType& operator[](T index) const {
		return data_[index];
	}

public:
	Matrix& operator=(const Matrix& mat);
	Matrix& operator=(Mat4x4&& mat) noexcept;
	Matrix operator*(const Mat4x4& mat) const;
	Matrix& operator*=(const Mat4x4& mat);

	Matrix operator+(const Mat4x4& mat) const;
	Matrix& operator+=(const Mat4x4& mat);
	Matrix operator-(const Mat4x4& mat) const;
	Matrix& operator-=(const Mat4x4& mat);

private:
	HeightType data_;
};