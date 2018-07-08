/*******************************
Copyright (c) 2016-2018 Gr�goire Angerand

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
#ifndef Y_UTILS_SORT_H
#define Y_UTILS_SORT_H

#if __has_include(<pdqsort.h>)
#include <pdqsort.h>
#define Y_USE_PDQSORT
#else
#include <algorithm>
// you can find pdqsort at https://github.com/orlp/pdqsort
#endif


namespace y {

template<typename It, typename Compare>
inline void sort(It begin, It end, Compare&& comp) {
#ifdef Y_USE_PDQSORT
	pdqsort(begin, end, std::forward<Compare>(comp));
#else
	std::sort(begin, end, std::forward<Compare>(comp));
#endif
}

template<typename It>
inline void sort(It begin, It end) {
#ifdef Y_USE_PDQSORT
	pdqsort(begin, end);
#else
	std::sort(begin, end);
#endif
}

}


#endif // Y_UTILS_SORT_H
