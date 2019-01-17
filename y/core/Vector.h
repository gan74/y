/*******************************
Copyright (c) 2016-2019 Grégoire Angerand

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
#ifndef Y_CORE_VECTOR_H
#define Y_CORE_VECTOR_H

#include "ArrayView.h"
#include <cstring>

namespace y {
namespace core {

struct DefaultVectorResizePolicy {
	static constexpr usize threshold = 8 * 1024;
	static constexpr usize minimum = 16;
	static constexpr usize step = 8 * 1024;

	usize ideal_capacity(usize size) const {
		if(!size) {
			return 0;
		}
		if(size < minimum) {
			return minimum;
		}
		if(size < threshold) {
			return 2 << (log2ui(size) + 1);
		}
		usize steps = (size - threshold) / step;
		return threshold + (steps + 1) * step;
	}

	bool shrink(usize size, usize capacity) const {
		unused(size, capacity);
		return false;
		//return !size || (capacity - size) > 2 * step;
	}
};


template<typename Elem, typename ResizePolicy = DefaultVectorResizePolicy, typename Allocator = std::allocator<Elem>>
class Vector : ResizePolicy, Allocator {

	using data_type = typename std::remove_const<Elem>::type;

	public:
		using value_type = Elem;
		using size_type = usize;

		using iterator = Elem*;
		using const_iterator = Elem const*;

		Vector() = default;

		Vector(const Vector& other) : Vector(other.begin(), other.end()) {
		}

		Vector(usize size, const value_type& elem) {
			set_min_capacity(size);
			std::fill_n(std::back_inserter(*this), size, elem);
		}

		template<typename It>
		Vector(const It& beg_it, const It& end_it) {
			assign(beg_it, end_it);
		}

		Vector(Vector&& other) {
			swap(other);
		}

		Vector(std::initializer_list<value_type> other) : Vector(other.begin(), other.end()) {
		}

		explicit Vector(ArrayView<value_type> other) : Vector(other.begin(), other.end()) {
		}

		template<typename... Args>
		Vector(const Vector<Elem, Args...>& other) : Vector(other.begin(), other.end()) {
		}

		Vector& operator=(Vector&& other) {
			swap(other);
			return *this;
		}

		Vector& operator=(const Vector& other) {
			if(&other != this) {
				if constexpr(std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
					clear();
					Allocator::operator=(other);
				}
				assign(other.begin(), other.end());
			}
			return *this;
		}

		Vector& operator=(std::initializer_list<value_type> l) {
			assign(l.begin(), l.end());
			return *this;
		}

		Vector& operator=(ArrayView<value_type> l) {
			if(contains_it(l.begin())) {
				Vector other(l);
				swap(other);
			} else {
				assign(l.begin(), l.end());
			}
			return *this;
		}

		template<typename... Args>
		Vector& operator=(const Vector<Elem, Args...>& l) {
			if(contains_it(l.begin())) {
				Vector other(l);
				swap(other);
			} else {
				assign(l.begin(), l.end());
			}
			return *this;
		}

		bool operator==(ArrayView<value_type> v) const {
			return size() == v.size() ? std::equal(begin(), end(), v.begin(), v.end()) : false;
		}

		bool operator!=(ArrayView<value_type> v) const {
			return !operator==(v);
		}

		template<typename... Args>
		bool operator==(const Vector<Elem, Args...>& v) const {
			return operator==(ArrayView<value_type>(v));
		}

		template<typename... Args>
		bool operator!=(const Vector<Elem, Args...>& v) const {
			return operator!=(ArrayView<value_type>(v));
		}


		template<typename It>
		void assign(const It& beg_it, const It& end_it) {
			make_empty();
			set_min_capacity(std::distance(beg_it, end_it));
			push_back(beg_it, end_it);
		}

		void swap(Vector& v) {
			if constexpr(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
				std::swap<Allocator>(*this, v);
			}
			std::swap(_data, v._data);
			std::swap(_data_end, v._data_end);
			std::swap(_alloc_end, v._alloc_end);
		}

		~Vector() {
			clear();
		}

		void push_back(const value_type& elem) {
			if(_data_end == _alloc_end) {
				expend();
			}
			new(_data_end++) data_type(elem);
		}

		void push_back(value_type&& elem) {
			if(_data_end == _alloc_end) {
				expend();
			}
			new(_data_end++) data_type(std::move(elem));
		}

		template<typename... Args>
		value_type& emplace_back(Args&&... args) {
			if(_data_end == _alloc_end) {
				expend();
			}
			return *(new(_data_end++) data_type(y_fwd(args)...));
		}

		template<typename It>
		void push_back(It beg_it, It end_it) {
			set_min_capacity(size() + std::distance(beg_it, end_it));
			std::copy(beg_it, end_it, std::back_inserter(*this));
		}

		template<typename It>
		void emplace_back(It beg_it, const It end_it) {
			set_min_capacity(size() + std::distance(beg_it, end_it));
			std::move(beg_it, end_it, std::back_inserter(*this));
		}

		value_type pop() {
			--_data_end;
			data_type r = std::move(*_data_end);
			_data_end->~data_type();
			shrink();
			return r;
		}

		void erase_unordered(iterator it) {
			if(it != end() - 1) {
				std::swap(*it, last());
			}
			pop();
		}

		void erase(iterator it) {
			std::move(it + 1, end(), it);
			pop();
		}

		usize size() const {
			return _data_end - _data;
		}

		bool is_empty() const {
			return _data == _data_end;
		}

		usize capacity() const {
			return _alloc_end - _data;
		}

		const_iterator begin() const {
			return _data;
		}

		const_iterator end() const {
			return _data_end;
		}

		const_iterator cbegin() const {
			return _data;
		}

		const_iterator cend() const {
			return _data_end;
		}

		iterator begin() {
			return _data;
		}

		iterator end() {
			return _data_end;
		}

		value_type* data() {
			return _data;
		}

		const value_type* data() const {
			return _data;
		}

		const value_type& operator[](usize i) const {
			return _data[i];
		}

		value_type& operator[](usize i) {
			return _data[i];
		}

		const value_type& first() const {
			return *_data;
		}

		value_type& first() {
			return *_data;
		}

		const value_type& last() const {
			return *(_data_end - 1);
		}

		value_type& last() {
			return *(_data_end - 1);
		}

		void set_capacity(usize cap) {
			unsafe_set_capacity(cap);
		}

		void set_min_capacity(usize min_cap) {
			unsafe_set_capacity(ResizePolicy::ideal_capacity(min_cap));
		}

		void reserve(usize cap) {
			if(capacity() < cap) {
				set_capacity(cap);
			}
		}

		void clear() {
			unsafe_set_capacity(0);
		}

		void squeeze() {
			set_capacity(size());
		}

		void make_empty() {
			clear(_data, _data_end);
			_data_end = _data;
		}

	private:
		static constexpr bool is_data_trivial = std::is_trivial_v<data_type>;

		bool contains_it(const_iterator it) const {
			return it >= _data && it < _data_end;
		}

		void move_range(data_type* dst, data_type* src, usize n) {
			if constexpr(is_data_trivial) {
				std::copy_n(src, n, dst);
			} else {
				for(; n; --n) {
					new(dst++) data_type(std::move(*(src++)));
				}
			}
		}

		void clear(data_type* beg, data_type* en) {
			if(!is_data_trivial) {
				while(en != beg) {
					(--en)->~data_type();
				}
			}
		}

		void expend() {
			unsafe_set_capacity(this->ideal_capacity(size() + 1));
		}

		void shrink() {
			usize current = size();
			usize cap = capacity();
			if(current < cap && ResizePolicy::shrink(current, cap)) {
				unsafe_set_capacity(ResizePolicy::ideal_capacity(current));
			}
		}

		// uses data_end !!
		void unsafe_set_capacity(usize new_cap) {
			usize current_size = size();
			usize num_to_move = std::min(new_cap, current_size);

			data_type* new_data = new_cap ? Allocator::allocate(new_cap) : nullptr;

			if(new_data != _data) {
				move_range(new_data, _data, num_to_move);
				clear(_data, _data_end);

				if(_data) {
					Allocator::deallocate(_data, capacity());
				}
			}

			_data = new_data;
			_data_end = _data + num_to_move;
			_alloc_end = _data + new_cap;
		}

		Owner<data_type*> _data = nullptr;
		data_type* _data_end = nullptr;
		data_type* _alloc_end = nullptr;
};

template<typename T>
inline auto vector_with_capacity(usize cap) {
	auto vec = Vector<T>();
	vec.set_min_capacity(cap);
	return vec;
}




template<typename... Args, typename T>
inline Vector<Args...>& operator<<(Vector<Args...>& vec, T&& t) {
	vec.push_back(y_fwd(t));
	return vec;
}

template<typename... Args, typename T>
inline Vector<Args...>& operator+=(Vector<Args...>& vec, T&& t) {
	vec.push_back(y_fwd(t));
	return vec;
}

template<typename... Args, typename T>
inline Vector<Args...> operator+(Vector<Args...> vec, T&& t) {
	vec.push_back(y_fwd(t));
	return vec;
}

template<typename T, usize = 0, typename... Args>
using SmallVector = Vector<T, Args...>; // for now...

}
}



#endif
