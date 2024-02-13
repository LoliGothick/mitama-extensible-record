#pragma once

#include <mitama/data/internal/record_helper.hpp>

#include <algorithm>
#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mitama
{
template <class, class = void>
struct named_as_impl : std::false_type
{
};

template <class _, auto Tag, class Expected>
  requires(named<Tag, _>::str == Expected::str)
struct named_as_impl<named<Tag, _>, Expected> : std::true_type
{
};

template <class _, auto Any>
struct named_as_impl<named<Any, _>, void> : std::true_type
{
};
} // namespace mitama

namespace mitama::inline where
{
template <class T, static_string Tag>
concept named_as = named_as_impl<T, named<Tag>>::value;

template <class T>
concept named_any = named_as_impl<std::remove_cvref_t<T>, void>::value;
} // namespace mitama::inline where

// Concepts for Extensible Records
namespace mitama::inline where
{
template <class... Named>
concept distinct = []
{
  if constexpr (sizeof...(Named) < 2)
    return true;
  else
  {
    std::array keys{ Named::str... };
    std::sort(keys.begin(), keys.end());
    return std::adjacent_find(keys.begin(), keys.end()) == keys.end();
  }
}();

template <class... Named>
concept is_sorted = []
{
  if constexpr (sizeof...(Named) < 2)
    return true;
  else
  {
    std::array keys{ Named::str... };
    return std::is_sorted(keys.begin(), keys.end());
  }
}();
} // namespace mitama::inline where

namespace mitama
{
template <named_any... Rows>
  requires distinct<Rows...>
class record;

template <class T>
struct make_record_impl;

template <named_any... Rows>
struct make_record_impl<type_list<Rows...>>
    : std::type_identity<record<Rows...>>
{
};

template <class List>
using make_record = make_record_impl<List>::type;
} // namespace mitama

// Extensible Records
namespace mitama
{
// forward declaration
template <named_any... Rows>
  requires distinct<Rows...>
class record;

template <class Record>
class shrink
{
  Record record;

public:
  template <class R>
  constexpr shrink(R&& r) : record(std::forward<R>(r))
  {
  }

  template <auto S>
  constexpr decltype(auto) operator[](static_string<S> tag) const
  {
    return record[tag];
  }
};

template <kind<of<record>> Record>
shrink(Record&&) -> shrink<Record>;

// record definition
template <named_any... Rows>
  requires distinct<Rows...>
class record : protected Rows...
{
  // private struct for private constructor
  template <named_any... From>
  struct FROM : protected std::remove_cvref_t<From>...
  {
    constexpr FROM(From&&... from)
        : std::remove_cvref_t<From>(std::forward<From>(from))...
    {
    }
    using std::remove_cvref_t<From>::operator[]...;
  };

  // private constructor
  template <named_any... Args>
  constexpr record(FROM<Args...> table) : Rows(table[Rows::tag])...
  {
  }

public:
  template <named_any... Args>
  constexpr record(Args&&... args)
      : record(FROM<Args...>(std::forward<Args>(args)...))
  {
  }

  template <mitamagic::superset_of<record> Record>
  constexpr record(shrink<Record> other) : Rows(other[Rows::tag])...
  {
  }

  template <static_string Key>
  using typeof = decltype(std::declval<FROM<Rows...>>()[Key]);

  using Rows::operator[]...;

  template <named_any... New>
  using spread = record<Rows..., New...>;

  template <static_string... Keys>
  using shrink = make_record<mitamagic::erased<type_list<Rows...>, Keys...>>;

  template <named_any Ex>
  constexpr auto operator+=(Ex ex) const
  {
    return spread<Ex>(Rows::clone()..., ex);
  }

  template <named_any... Ex>
  constexpr auto operator+=(std::tuple<Ex...> ex) const
  {
    return std::apply(
        [](auto&&... ex)
        {
          return spread<std::remove_cvref_t<Ex>...>{
            Rows::clone()..., std::forward<decltype(ex)>(ex)...
          };
        },
        ex
    );
  }

  constexpr auto dissolve() const& noexcept
  {
    return std::forward_as_tuple(
        std::forward<typeof<std::remove_cvref_t<Rows>::tag>>(
            this->operator[](std::remove_cvref_t<Rows>::tag)
        )...
    );
  }
};

template <named_any... Rows>
record(Rows...) -> record<Rows...>;

template <>
class record<>
{
};

inline constexpr record<> empty_record{};

template <named_any Row>
inline constexpr auto
operator+=(record<>, Row named)
{
  return record{ named };
}

template <named_any... Ex>
constexpr auto
operator+=(record<>, std::tuple<Ex...> ex)
{
  return std::apply(
      [](auto&&... ex)
      {
        return record<std::remove_cvref_t<Ex>...>{ std::forward<decltype(ex)>(ex
        )... };
      },
      ex
  );
}

template <static_string... S>
inline constexpr auto select = [](const auto& rec)
{ return std::forward_as_tuple(rec[S]...); };
} // namespace mitama

namespace mitama::inline where
{
template <class Record, static_string... Required>
concept has_rows = []<named_any... Rows>(std::type_identity<record<Rows...>>)
{
  return []<named_any... Sorted>(type_list<Sorted...>)
  {
    std::array tags = { Sorted::str... };
    return (
        std::binary_search(
            tags.cbegin(), tags.cend(), decltype(Required)::value
        )
        && ...
    );
  }(mitamagic::sorted<Rows...>());
}(std::type_identity<Record>());
}
