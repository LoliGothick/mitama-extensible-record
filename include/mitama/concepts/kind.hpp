#pragma once

#include <ranges>
#include <type_traits>

namespace mitama::mitamagic
{
template <template <class...> class, class>
struct of_impl : std::false_type
{
};
template <template <class...> class Expected, class... _>
struct of_impl<Expected, Expected<_...>> : std::true_type
{
};
} // namespace mitama::mitamagic

namespace mitama
{
template <template <class...> class Expected>
struct of
{
  template <class T>
  static constexpr bool value =
      mitamagic::of_impl<Expected, std::remove_cvref_t<T>>::value;
};

template <class T, class Of>
concept kind = Of::template value<T>;
} // namespace mitama
