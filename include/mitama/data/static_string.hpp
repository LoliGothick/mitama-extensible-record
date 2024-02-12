#include <cstddef>
#include <string_view>
#include <type_traits>

namespace mitama
{
template <std::size_t N, class CharT>
struct fixed_string
{
  static constexpr std::size_t size = N;
  using char_type = CharT;

  constexpr fixed_string(const CharT (&s)[N])
      : fixed_string(s, std::make_index_sequence<N>{})
  {
  }
  template <std::size_t... Indices>
  constexpr fixed_string(const CharT (&s)[N], std::index_sequence<Indices...>)
      : s{ s[Indices]... }
  {
  }

  const CharT s[N];
};

template <std::size_t N, class CharT>
std::ostream&
operator<<(std::ostream& os, fixed_string<N, CharT> fs)
{
  return os << fs.s;
}
} // namespace mitama

namespace mitama
{
// non-type template enabled static string class
//
// S: fixed_string<N, CharT>
template <fixed_string S>
struct static_string
{
  using char_type = typename decltype(S)::char_type;
  static constexpr const std::basic_string_view<char_type> value = {
    S.s, decltype(S)::size
  };

  template <auto T>
    requires std::same_as<char_type, typename static_string<T>::char_type>
  constexpr std::strong_ordering operator<=>(static_string<T>) const noexcept
  {
    return static_string::value <=> static_string<T>::value;
  }

  constexpr operator std::basic_string_view<char_type>() const
  {
    return value;
  }
};

template <auto S>
std::ostream&
operator<<(std::ostream& os, static_string<S>)
{
  return os << static_string<S>::value;
}
} // namespace mitama

namespace mitama
{
template <class T>
struct is_static_str : std::false_type
{
};
template <auto S>
struct is_static_str<static_string<S>> : std::true_type
{
};

template <class T>
concept static_strings = is_static_str<std::remove_cvref_t<T>>::value;
} // namespace mitama

namespace mitama::inline literals::inline static_string_literals
{
// static string literal
template <fixed_string S>
inline constexpr static_string<S>
operator""_()
{
  return {};
}
} // namespace mitama::inline literals::inline static_string_literals
