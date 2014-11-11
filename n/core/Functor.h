/*******************************
Copyright (C) 2013-2014 gr�goire ANGERAND

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

#ifndef N_CORE_FUNCTOR_H
#define N_CORE_FUNCTOR_H

#include <n/types.h>
#include <tuple>

#include "SmartPtr.h"

namespace n {

namespace internal {

template<typename R, uint N>
struct TupleProxy {
	template<typename T, typename... TArgs, typename... Args>
	static R apply(T *t, const std::tuple<TArgs...> &tuple, Args... args) {
		return TupleProxy<R, N - 1>::apply(t, tuple, std::get<N - 1>(tuple), args...);
	}
};
template<typename R>
struct TupleProxy<R, 0> {
	template<typename T, typename... TArgs, typename... Args>
	static R apply(T *t, const std::tuple<TArgs...> &, Args... args) {
		return (*t)(args...);
	}
};

}


namespace core {

template<typename R, typename... Args>
class Functor
{
	class FuncBase
	{
		public:
			FuncBase() {
			}

			FuncBase(const FuncBase &) = delete;

			virtual ~FuncBase() {
			}

			virtual R apply(Args...) = 0;
	};

	template<typename T>
	class Func : public FuncBase
	{
		public:
			Func(const T &t) : f(t) {
			}

			Func(const Func<T> &) = delete;

			virtual R apply(Args... args) override {
				return f(args...);
			}

		private:
			T f;
	};

	class FuncPtr : public FuncBase
	{
		public:
			FuncPtr(R (*t)(Args...)) : f(t) {
			}

			FuncPtr(const FuncPtr &) = delete;

			virtual R apply(Args... args) override {
				return f(args...);
			}

		private:
			R (*f)(Args...);
	};

	class Curry
	{
		public:
			Curry(const Functor<R, Args...> &f, Args... ar) : func(f), args(ar...) {
			}

			Curry(const Functor<R, Args...> &f, std::tuple<Args...> ar) : func(f), args(ar) {
			}

			R operator()() {
				return func(args);
			}

		private:
			Functor<R, Args...> func;
			std::tuple<Args...> args;

	};

	public:
		typedef R ReturnType;

		template<typename T>
		Functor(const T &t) : func(new Func<T>(t)) {
		}

		Functor(R (*f)(Args...)) : func(new FuncPtr(f)) {
		}

		R operator()(Args... args) {
			return func->apply(args...);
		}

		R operator()(const std::tuple<Args...> &args) {
			return n::internal::TupleProxy<R, sizeof...(Args)>::apply(this, args);
		}

		Functor<R> curried(Args... args) const {
			return Functor<R>(Curry(*this, args...));
		}

		Functor<R> curried(std::tuple<Args...> args) const {
			return Functor<R>(Curry(*this, args));
		}

	private:
		SmartPtr<FuncBase> func;

};

template<typename R, typename... Args>
class Function {};

template<typename R, typename... Args>
class Function<R(Args...)>
{
	public:
		static constexpr uint args = sizeof...(Args);

		template<uint N>
		using argAt = typename std::tuple_element<N, std::tuple<Args...>>::type;

		using functor = Functor<R, Args...>;
};




}
}

#endif // FUNCTOR_H
