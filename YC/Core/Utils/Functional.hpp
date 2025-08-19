#pragma once

#include <concepts>
#include <variant>
#include <YC/ErrorOr/ErrorOr.hpp>

template<typename ... Ts>                                                 
struct Overload : Ts ... { 
	using Ts::operator() ...;
};
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

template <typename T>
struct fn_first_args_type;

template <typename ReturnType, typename... Args>
struct fn_first_args_type<ReturnType(Args...)> {
	using type = std::tuple_element_t<0, std::tuple<Args...>>;
};

template<typename Ret, typename Class, typename... Args>
struct fn_first_args_type<Ret (Class::*)(Args...) const>
{
	using type = std::tuple_element_t<0, std::tuple<Args...>>;
};
template <typename Callable>
struct fn_first_args_type : fn_first_args_type<decltype(&Callable::operator())> {};

template <typename T>
using fn_first_args_type_t = typename fn_first_args_type<T>::type;

template <typename T>
struct is_variant : std::false_type {};

template <typename... Ts>
struct is_variant<std::variant<Ts...>> : std::true_type {};

template <typename T>
concept Variant = is_variant<T>::value;

template <Variant VarType, typename FnType>
requires is_t_err_opt<std::invoke_result_t<FnType, fn_first_args_type_t<FnType>>>
static auto operator| (const VarType& Var, const FnType& Fn) -> decltype(Fn({})) {
	using FnArgsType = fn_first_args_type_t<FnType>;
	return std::visit([&Fn]<typename T0>(T0&& Args) -> decltype(Fn({})) {
			if constexpr (std::is_same_v<std::decay_t<T0>, FnArgsType>) return Fn(Args);
			else return Err { std::string("Not Matched Variant!") };
	}, Var);
}

template <Variant VarType, typename FnType>
requires !is_t_err_opt<std::invoke_result_t<FnType, fn_first_args_type_t<FnType>>>
&& !is_return_type_void<FnType, fn_first_args_type_t<FnType>>
static auto operator| (const VarType& Var, const FnType& Fn) -> ErrorOr<decltype(Fn({})), std::string> {
	using FnArgsType = fn_first_args_type_t<FnType>;
	return std::visit([&Fn]<typename T0>(T0&& Args) -> ErrorOr<decltype(Fn({})), std::string> {
			if constexpr (std::is_same_v<std::decay_t<T0>, FnArgsType>) return Fn(Args);
			else return Err { std::string("Not Matched Variant!") };
	}, Var);
}

template <Variant VarType, typename FnType>
requires !is_t_err_opt<std::invoke_result_t<FnType, fn_first_args_type_t<FnType>>>
&& is_return_type_void<FnType, fn_first_args_type_t<FnType>>
static auto operator| (const VarType& Var, const FnType& Fn) -> ErrorOr<void> {
	using FnArgsType = fn_first_args_type_t<FnType>;
	return std::visit([&Fn]<typename T0>(T0&& Args) -> ErrorOr<void, std::string> {
			if constexpr (std::is_same_v<std::decay_t<T0>, FnArgsType>) {
				Fn(Args);
				return {};
			}
			else return Err { std::string("Not Matched Variant!") };
	}, Var);
}

namespace YC_Impl {

template <typename ErrorType>
struct fn_or_type_get_error_t {
	ErrorType Error;
	bool IsError = true;
};

template <typename ValueType>
struct fn_or_type_get_value_t {
	ValueType Value;
	bool IsOk = true;
};

struct __unwrap {};
}

template <typename FnType, typename ValueType, typename ErrorOrType>
concept is_function_return_type_fn_or_type_get_value_t = requires(FnType Fn, ErrorOrType Value) {
	{ Fn(Value) } -> std::same_as<YC_Impl::fn_or_type_get_value_t<ValueType>>;
};
template <typename FnType, typename ErrorType, typename ErrorOrType>
concept is_function_return_type_fn_or_type_get_error_t = requires(FnType Fn, ErrorOrType Value) {
	{ Fn(Value) } -> std::same_as<YC_Impl::fn_or_type_get_error_t<ErrorType>>;
};

template <typename Type, typename ErrorType, typename F>
requires is_function_return_type_fn_or_type_get_value_t<F, Type, ErrorOr<Type, ErrorType>>
auto operator| (const ErrorOr<Type, ErrorType>& Left, F&& Fn) -> decltype(Fn(Left)) {
	return Fn(Left);
}

template <typename Type, typename ErrorType, typename F>
requires is_function_return_type_fn_or_type_get_error_t<F, ErrorType, ErrorOr<Type, ErrorType>>
auto operator| (const ErrorOr<Type, ErrorType>& Left, F&& Fn) -> decltype(Fn(Left)) {
	return Fn(Left);
}

inline auto Unwrap = YC_Impl::__unwrap {};

template <typename Type, typename ErrorType>
auto operator| (const ErrorOr<Type, ErrorType>& Left, YC_Impl::__unwrap) -> decltype(Left.Unwrap()) {
	return Left.Unwrap();
}


template <typename ErrorType, typename F>
requires is_return_type_void<F, ErrorType>
auto operator| (YC_Impl::fn_or_type_get_error_t<ErrorType> Left, F&& Fn) -> void {
	if(Left.IsError) {
		Fn(Left.Error);
	}
}

template <typename ErrorType, typename F>
requires is_return_type_void<F>
auto operator| (YC_Impl::fn_or_type_get_error_t<ErrorType> Left, F&& Fn) -> void {
	if(Left.IsError) {
		Fn();
	}
}

template <typename ErrorType, typename F>
requires !is_return_type_void<F, ErrorType>
auto operator| (YC_Impl::fn_or_type_get_error_t<ErrorType> Left, F&& Fn) -> decltype(Fn(Left.Error)) {
	if(Left.IsError) {
		return Fn(Left.Error);
	}
	return {};
}

template <typename Type, typename F>
requires is_return_type_void<F>
auto operator| (YC_Impl::fn_or_type_get_value_t<Type> Left, F&& Fn) -> void {
	if(Left.IsOk) {
		Fn();
	}
}

template <typename Type, typename F>
requires is_return_type_void<F, Type>
auto operator| (YC_Impl::fn_or_type_get_value_t<Type> Left, F&& Fn) -> void {
	if(Left.IsOk) {
		Fn(Left.Value);
	}
}

template <typename Type, typename F>
requires !is_return_type_void<F, Type>
auto operator| (YC_Impl::fn_or_type_get_value_t<Type> Left, F&& Fn) -> decltype(Fn(Left.Value)) {
	if(Left.IsOk) {
		return Fn(Left.Value);
	}
	return {};
}

inline auto WhenOk = [](is_t_err_opt auto Left) -> YC_Impl::fn_or_type_get_value_t<decltype(Left.Unwrap())> {
	if(Left.IsOk()) return { Left.Unwrap() };
	return { {}, false };
};
inline auto WhenErr = [](is_t_err_opt auto Left) -> YC_Impl::fn_or_type_get_error_t<decltype(Left.GetError())> {
	if(Left.IsErr()) return { Left.GetError() };
	return { {}, false };
};