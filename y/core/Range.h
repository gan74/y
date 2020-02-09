/*******************************
Copyright (c) 2016-2020 Grégoire Angerand

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
#ifndef Y_CORE_RANGE_H
#define Y_CORE_RANGE_H

#include <y/utils.h>

namespace y {
namespace core {

template<typename Iter, typename EndIter = Iter>
class Range {
	public:
		using iterator_traits = std::iterator_traits<Iter>;

		using iterator = Iter;
		using const_iterator = Iter;

		Range(Iter b, EndIter e) : _beg(b), _end(e) {
		}

		template<typename Coll>
		Range(const Coll& col) : Range(col.begin(), col.end()) {
		}

		template<typename Coll>
		Range(Coll& col) : Range(col.begin(), col.end()) {
		}

		Iter begin() const {
			return _beg;
		}

		EndIter end() const {
			return _end;
		}

		usize size() const {
			if constexpr(std::is_same_v<Iter, EndIter>) {
				return std::distance(_beg, _end);
			} else {
				usize s = 0;
				for(auto&& k : *this) {
					unused(k);
					++s;
				}
				return s;
			}
		}

	private:
		Iter _beg;
		EndIter _end;
};

template<typename Coll>
Range(const Coll&) -> Range<typename Coll::const_iterator>;

template<typename Coll>
Range(Coll&) -> Range<typename Coll::iterator>;

}
}

#endif // Y_CORE_RANGE_H
