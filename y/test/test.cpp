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

#include "test.h"
#include <y/utils.h>
#include <cstring>

#include <iostream>

namespace y {
namespace test {
namespace detail {


const char* test_box_msg(const char* msg) {
	return msg ? msg : "unknown test function";
}

void test_assert(const char* msg, void (*func)(TestResult &)) {
	const char* ok		= "\x1b[32m  [ OK ]   \x1b[0m";
	const char* failure = "\x1b[31m[ FAILED ] \x1b[0m";

	// Because of this function may be called during static initialization we must initialize cout
	const std::ios_base::Init init;
	y::detail::setup_console();


	std::cout << msg << ":";
	for(usize size = strlen(msg) + 1; size != 80 - 11; ++size) {
		std::cout << " ";
	}


	TestResult res{true, nullptr, 0};
	func(res);

	if(res.result) {
		std::cout << ok << std::endl;
	} else {
		std::cout << failure << std::endl;
		fatal("\ty_test_assert failed!", res.file, res.line);
	}
	unused(msg, func);
}

}
}
}
