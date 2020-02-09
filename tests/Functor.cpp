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

#include <y/core/Functor.h>
#include <y/test/test.h>

namespace {
using namespace y;
using namespace y::core;

auto create_func() {
	return []() {
		return 4;
	};
}

void forward_func(int&& i) {
	++i;
}

struct NonConstFunctorStruct {
	int operator()(int i) {
		return i - 1;
	}
};

struct ForwardStruct {
	int operator()(int&& i) {
		return --i;
	}
};

static int function_trais_test(int a, double, core::Functor<void()>&) {
	return a;
}

static_assert(std::is_same_v<function_traits<decltype(function_trais_test)>::return_type, int>);
static_assert(std::is_same_v<function_traits<decltype(function_trais_test)>::arg_type<0>, int>);
static_assert(std::is_same_v<function_traits<decltype(function_trais_test)>::arg_type<1>, double>);
static_assert(std::is_same_v<function_traits<decltype(function_trais_test)>::arg_type<2>, core::Functor<void()>&>);

template<typename F>
inline auto function(F&& f) {
	using traits = function_traits<typename std::remove_reference<F>::type>;
	return core::Function<typename traits::func_type>(y_fwd(f));
}


template<typename F>
inline auto functor(F&& f) {
	using traits = function_traits<typename std::remove_reference<F>::type>;
	return core::Functor<typename traits::func_type>(y_fwd(f));
}

y_test_func("Function creation") {
	int i = 0;
	auto inc = function([&i]() { ++i; });

	y_test_assert(!i);
	inc();
	y_test_assert(i == 1);

	{
		inc = function([&i]() { --i; });
	}

	inc();
	y_test_assert(!i);

	{
		const auto dec = std::move(inc);
		dec();
		y_test_assert(i == -1);
	}

	inc = create_func();
	inc();
	y_test_assert(i == -1);
}

y_test_func("Function method creation") {
	{
		NonConstFunctorStruct t;
		const auto func = function(t);
		y_test_assert(func(4) == 3);
	}
	{
		ForwardStruct t;
		const auto func = function(t);
		y_test_assert(func(4) == 3);
		int i = 7;
		// should not work
		//y_test_assert(func(i) == 6);
		y_test_assert(func(std::move(i)) == 6 && i == 6);
	}
}

y_test_func("Function argument forwarding") {
	const auto func = function(forward_func);
	int i = 4;
	// should not work
	// func(i);
	func(4);
	func(std::move(i));
	y_test_assert(i == 5);
}

y_test_func("Function void boxing") {
	int i = 0;
	const auto inc = function([&i]() { return ++i; });
	inc();
	y_test_assert(i == 1);
}

y_test_func("Functor creation") {
	int i = 0;
	const auto inc = functor([&i]() { ++i; });

	inc();
	y_test_assert(i == 1);

	auto dec = inc;

	dec();
	y_test_assert(i == 2);
	{
		const auto d = functor([&i]() { --i; });
		dec = d;
	}
	dec();
	dec();
	y_test_assert(!i);

	dec = create_func();
	dec();
	y_test_assert(!i);

}
}
