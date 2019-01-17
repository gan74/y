/*******************************
Copyright (c) 2016-2019 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/
#ifndef Y_MATH_TRANSFORM_H
#define Y_MATH_TRANSFORM_H


#include "Matrix.h"
#include "Quaternion.h"

namespace y {
namespace math {

template<typename T = float>
struct Transform : Matrix4<T> {

	Transform() : Matrix4<T>(identity()) {
	}

	Transform(const Matrix4<T>& m) : Matrix4<T>(m) {
	}

	Transform(const Vec<3, T>& pos) : Matrix4<T>(identity()) {
		position() = pos;
	}

	Transform(const Vec<3, T>& pos, const Quaternion<T>& rotation, const Vec<3, T>& scale = {1, 1, 1}) {
		this->column(0).template to<3>() = rotation({scale.x(), 0, 0});
		this->column(1).template to<3>() = rotation({0, scale.y(), 0});
		this->column(2).template to<3>() = rotation({0, 0, scale.z()});
		this->column(3) = Vec<4, T>(pos, 1);
	}

	using Matrix4<T>::operator*;

	operator Matrix4<T>() const {
		return *this;
	}

	/*auto right() const {
		return -this->column(1).template to<3>();
	}*/

	const auto& forward() const {
		return this->column(0).template to<3>();
	}

	const auto& left() const {
		return this->column(1).template to<3>();
	}

	const auto& up() const {
		return this->column(2).template to<3>();
	}


	const auto& position() const {
		return this->column(3).template to<3>();
	}

	auto& position() {
		return this->column(3).template to<3>();
	}


	void set_basis(const Vec<3, T>& forward, const Vec<3, T>& left, const Vec<3, T>& up) {
		this->column(0).template to<3>() = forward;
		this->column(1).template to<3>() = left;
		this->column(2).template to<3>() = up;
	}

	void set_basis(const Vec<3, T>& forward, const Vec<3, T>& up) {
		set_basis(forward, forward.cross(up), up);
	}

	std::tuple<Vec<3, T>,  Quaternion<T>, Vec<3, T>> decompose() const {
		const auto& x = this->column(0).template to<3>();
		const auto& y = this->column(1).template to<3>();
		const auto& z = this->column(2).template to<3>();
		Vec<3, T> scale = {x.length(), y.length(), z.length()};
		return {position(), Quaternion<T>::from_base(x / scale.x(), y / scale.y(), z / scale.z()), scale};
	}

};


static_assert(sizeof(Transform<>) == sizeof(Matrix4<>), "Transfrom<T> should have the same size as Matrix4<T>");
static_assert(std::is_trivially_copyable_v<Transform<>>, "Transfrom<T> should be trivially copyable");

}
}

#endif // Y_MATH_TRANSFORM_H
