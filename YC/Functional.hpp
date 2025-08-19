#pragma once

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <concepts>

template<class... Ts> struct MultiCallable : Ts... { using Ts::operator()...; };
template<class... Ts> MultiCallable(Ts...)->MultiCallable<Ts...>;

namespace YC {
}
