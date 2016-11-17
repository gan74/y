/*******************************
Copyright (C) 2013-2016 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/
#ifndef Y_MATH_QUATERNION_H
#define Y_MATH_QUATERNION_H

#include "math.h"
#include "Vec.h"

#include <limits>

namespace y {
namespace math {
namespace experimental {

template<typename T = float>
class Quaternion {

	static_assert(std::is_floating_point<T>::value, "Quaternion only support floating point types.");

	public:

		static constexpr usize YawIndex = 2;
		static constexpr usize PitchIndex = 1;
		static constexpr usize RollIndex = 0;

		template<typename U>
		Quaternion(const Vec<4, U>& q) : _quat(q.normalized()) {
		}

		template<typename... Args>
		Quaternion(Args&&... args) : Quaternion(Vec<4, T>(std::forward<Args>(args)...)) {
		}

		Quaternion(detail::identity_t i = identity()) : Quaternion(0, 0, 0, 1) {
		}

		template<typename U>
		Quaternion& operator=(const Vec<4, U>& q) {
			_quat = q.normalized();
			return *this;
		}

		template<typename U>
		Quaternion& operator=(const Quaternion<U>& q) {
			_quat = q._quat;
			return *this;
		}



		T angle() const {
			return std::acos(w() * T(2));
		}

		Vec<3, T> axis() const {
			return Vec<3, T>(_quat.sub(3) / std::sqrt(T(1) - w() * w()));
		}

		Quaternion inverse() const {
			return Quaternion(-_quat.sub(3), _quat.w());
		}

		explicit operator Vec<4, T>() const {
			return _quat;
		}

		Vec<3, T> operator()(const Vec<3, T>& v) const {
			Vec<3, T> u = _quat.sub(3);
			return u * T(2) * u.dot(v)
				  + v * (w() * w() - u.length2())
				  + u.cross(v) * T(2) * w();
		}

		T x() const {
			 return _quat.x();
		}

		T y() const {
			 return _quat.y();
		}

		T z() const {
			 return _quat.z();
		}

		T w() const {
			 return _quat.w();
		}

		Vec<3, T> to_euler() const {
			return Vec<3, T>(std::atan2(T(2) * (w() * x() + y() * z()), T(1) - T(2) * (x() * x() + y() * y())),
							 std::asin(T(2) * (w() * y() - z() * x())),
							 std::atan2(T(2) * (w() * z() + x() * y()), T(1) - T(2) * (y() * y() + z() * z())));
		}

		Vec<4, T> to_axis_angle() const {
			T s = std::sqrt(T(w) - w() * w());
			if(s < epsilon<T>()) {
				s = T(1);
			}
			return Vec<4, T>(_quat.sub(3) / s, std::acos(w()) * T(2));
		}



		Quaternion operator-() const {
			return inverse();
		}

		template<typename U>
		Quaternion& operator*=(const U& s) const {
			operator=(*this * s);
			return *this;
		}

		template<typename U>
		Quaternion& operator/=(const U& s) const {
			operator=(*this / s);
			return *this;
		}

		Quaternion operator*(const Quaternion& q) const {
			return Quaternion(w() * q.x() + x() * q.w() + y() * q.z() - z() * q.y(),
							  w() * q.y() + y() * q.w() + z() * q.x() - x() * q.z(),
							  w() * q.z() + z() * q.w() + x() * q.y() - y() * q.x(),
							  w() * q.w() - x() * q.x() - y() * q.y() - z() * q.z());
		}

		Quaternion operator*(T s) const {
			return Quaternion(_quat * s);
		}

		Quaternion operator/(T s) const {
			return Quaternion<T>(_quat / s);
		}


		static Quaternion look_at(Vec<3, T> f) {
			f.normalize();
			Vec<3, T> axis(1, 0, 0);
			T d = f.dot(axis);
			if(std::abs(d + T(1.0)) < epsilon<T>()) {
				return from_axis_angle(Vec<3, T>(0, 0, 1), pi<T>);
			} else if(std::abs(d - T(1.0)) < epsilon<T>()) {
				return Quaternion();
			}
			return from_axis_angle(f.cross(axis), -std::acos(d));
		}

		static Quaternion from_euler(T yaw, T pitch, T roll) {
			T cos_yaw = std::cos(pitch * T(0.5));
			T sin_yaw = std::sin(pitch * T(0.5));
			T cos_pitch = std::cos(roll * T(0.5));
			T sin_pitch = std::sin(roll * T(0.5));
			T cos_roll = std::cos(yaw * T(0.5));
			T sin_roll = std::sin(yaw * T(0.5));
			return Quaternion(cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw,
							  cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw,
							  sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw,
							  cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw);
		}

		static Quaternion from_base(const Vec<3, T>& forward, const Vec<3, T>& side, const Vec<3, T>& up) {
			T w = std::sqrt(1 + forward.x() + side.y() + up.z()) * T(0.5);
			Vec<3, T> q(side.z() - up.y(), up.x() - forward.z(), forward.y() - side.x());
			return Quaternion(q / (4 * w), w);
		}

		static Quaternion from_euler(const Vec<3, T>& euler) {
			return fromEuler(euler[YawIndex], euler[PitchIndex], euler[RollIndex]);
		}

		static Quaternion from_axis_angle(const Vec<3, T>& axis, T ang) {
			T s = std::sin(ang * T(0.5)) / axis.length();
			return Quaternion(axis.x() * s, axis.y() * s, axis.z() * s, std::cos(ang * T(0.5)));
		}

		static Quaternion from_axis_angle(const Vec<3, T>& axis_ang) {
			return from_axis_angle(axis_ang, axis_ang.length());
		}

		static Quaternion from_axis_angle(const Vec<4, T>& v) {
			return from_axis_angle(v.sub(3), v.w());
		}

	private:
		Vec<4, T> _quat;
};

}
}
}

#endif // Y_MATH_QUATERNION_H