#pragma once

#include <string>
#include <string_view>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <print>
#include <format>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#define fat double

#define ywlib_compare_op(Name, Op) struct Name { constexpr bool operator()(auto&& a, auto&& b) const ywlib_wrap_auto(bool(a Op b)); }
#define ywlib_wrap_auto(...) noexcept(noexcept(__VA_ARGS__)) requires requires { __VA_ARGS__; } { return __VA_ARGS__; }
#define ywlib_wrap_void(...) noexcept(noexcept(__VA_ARGS__)) requires requires { __VA_ARGS__; } { __VA_ARGS__; }
#define ywlib_wrap_ref(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(auto) requires requires { __VA_ARGS__; } { return __VA_ARGS__; }

namespace yw { // core

inline constexpr int npos = -1;
inline constexpr auto unordered = std::partial_ordering::unordered;

ywlib_compare_op(equal, ==);
ywlib_compare_op(not_equal, !=);
ywlib_compare_op(less, <);
ywlib_compare_op(greater, >);
ywlib_compare_op(less_equal, <=);
ywlib_compare_op(greater_equal, >=);

inline constexpr equal eq;
inline constexpr not_equal ne;
inline constexpr less lt;
inline constexpr greater gt;
inline constexpr less_equal le;
inline constexpr greater_equal ge;

template<typename T, typename... Ts> concept same_as = (std::same_as<T, Ts> && ...);
template<typename T, typename... Ts> concept included_in = (std::same_as<T, Ts> || ...);
template<typename T, typename... Ts> concept different_from = !included_in<T, Ts...>;
template<typename T, typename... Ts> concept derived_from = (std::derived_from<T, Ts> && ...);
template<typename T, typename... Ts> concept castable_to = requires(T (&f)()) { ((static_cast<Ts>(f())), ...); };
template<typename T, typename... Ts> concept nt_castable_to = requires (T (&f)() noexcept) { { ((static_cast<Ts>(f())), ...) } noexcept; };
template<typename T, typename... Ts> concept convertible_to = (std::convertible_to<T, Ts> && ...) && castable_to<T, Ts...>;
template<typename T, typename... Ts> concept nt_convertible_to = convertible_to<T, Ts...> && nt_castable_to<T, Ts...>;

template<auto V, typename T = decltype(V)> requires convertible_to<decltype(V), T> struct constant {
  using type = T;
  static constexpr type value = V;
  consteval operator type() const noexcept { return value; }
  consteval type operator()() const noexcept { return value; }
};

struct null_t {
  constexpr null_t() noexcept = default;
  constexpr null_t(auto&&...) noexcept {}
  constexpr null_t& operator=(auto&&) noexcept { return *this; }
  explicit constexpr operator bool() const noexcept { return false; }
  constexpr null_t operator()() const noexcept { return {}; }
  friend constexpr bool operator==(null_t, null_t) noexcept { return false; }
  friend constexpr auto operator<=>(null_t, null_t) noexcept { return unordered; }
  friend constexpr null_t operator+(null_t) noexcept { return {}; }
  friend constexpr null_t operator-(null_t) noexcept { return {}; }
  friend constexpr null_t operator+(null_t, null_t) noexcept { return {}; }
  friend constexpr null_t operator-(null_t, null_t) noexcept { return {}; }
  friend constexpr null_t operator*(null_t, null_t) noexcept { return {}; }
  friend constexpr null_t operator/(null_t, null_t) noexcept { return {}; }
  constexpr null_t& operator+=(null_t) noexcept { return *this; }
  constexpr null_t& operator-=(null_t) noexcept { return *this; }
  constexpr null_t& operator*=(null_t) noexcept { return *this; }
  constexpr null_t& operator/=(null_t) noexcept { return *this; }
};
inline constexpr null_t null{};

template<typename T> using remove_const = std::remove_const_t<T>;
template<typename T> using remove_volatile = std::remove_volatile_t<T>;
template<typename T> using remove_cv = std::remove_cv_t<T>;
template<typename T> using remove_ref = std::remove_reference_t<T>;
template<typename T> using remove_cvref = std::remove_cvref_t<T>;
template<typename T> using remove_pointer = std::remove_pointer_t<T>;
template<typename T> using remove_extent = std::remove_extent_t<T>;

template<typename T> concept is_const = std::is_const_v<T>;
template<typename T> concept is_volatile = std::is_volatile_v<T>;
template<typename T> concept is_cv = is_const<T> && is_volatile<T>;
template<typename T> concept is_lvref = std::is_lvalue_reference_v<T>;
template<typename T> concept is_rvref = std::is_rvalue_reference_v<T>;
template<typename T> concept is_reference = is_lvref<T> || is_rvref<T>;
template<typename T> concept is_pointer = std::is_pointer_v<T>;
template<typename T> concept is_bounded_array = std::is_bounded_array_v<T>;
template<typename T> concept is_unbounded_array = std::is_unbounded_array_v<T>;
template<typename T> concept is_array = is_bounded_array<T> || is_unbounded_array<T>;
template<typename T> concept is_function = std::is_function_v<T>;

template<typename T> struct t_is_member_pointer : constant<false> {};
template<typename M, typename C> struct t_is_member_pointer<M C::*> : constant<true> { using class_type = C; using member_type = M; };
template<typename M, typename C> struct t_is_member_pointer<M C::* const> : t_is_member_pointer<M C::*> {};
template<typename M, typename C> struct t_is_member_pointer<M C::* volatile> : t_is_member_pointer<M C::*> {};
template<typename M, typename C> struct t_is_member_pointer<M C::* const volatile> : t_is_member_pointer<M C::*> {};
template<typename T> concept is_member_pointer = t_is_member_pointer<T>::value && is_pointer<T>;
template<is_member_pointer T> using class_type = typename t_is_member_pointer<T>::class_type;
template<is_member_pointer T> using member_type = typename t_is_member_pointer<T>::member_type;
template<typename T> concept is_member_function_pointer = is_member_pointer<T> && is_function<member_type<T>>;
template<typename T> concept is_member_object_pointer = is_member_pointer<T> && !is_member_function_pointer<T>;

template<typename T> concept is_enum = std::is_enum_v<T>;
template<typename T> concept is_scoped_enum = is_enum<T> && !convertible_to<T, bool>;
template<is_enum T> using underlying_type = std::underlying_type_t<T>;
inline constexpr auto to_underlying = [](is_enum auto e) noexcept { return static_cast<underlying_type<decltype(e)>>(e); };

template<typename T> concept is_class = std::is_class_v<T>;
template<typename T> concept is_union = std::is_union_v<T>;
template<typename T> concept is_abstract = is_class<T> && std::is_abstract_v<T>;
template<typename T> concept is_aggregate = std::is_aggregate_v<T>;
template<typename T> concept is_empty = is_class<T> && std::is_empty_v<T>;
template<typename T> concept is_final = is_class<T> && std::is_final_v<T>;
template<typename T> concept is_polymorphic = is_class<T> && std::is_polymorphic_v<T>;
template<typename T> concept has_virtual_destructor = is_class<T> && std::has_virtual_destructor_v<T>;
template<typename T> concept standard_layout = std::is_standard_layout_v<T>;
template<typename T> concept trivially_copyable = std::is_trivially_copyable_v<T>;

template<typename T> concept integral = std::integral<T>;
template<typename T> concept signed_integral = std::signed_integral<T>;
template<typename T> concept unsigned_integral = std::unsigned_integral<T>;
template<typename T> concept floating_point = std::floating_point<T>;
template<typename T> concept arithmetic = integral<T> || floating_point<T>;

template<typename T> concept is_void = same_as<remove_cv<T>, void>;
template<typename T> concept is_bool = same_as<remove_cv<T>, bool>;
template<typename T> concept is_null = same_as<remove_cv<T>, null_t>;
template<typename T> concept is_nullptr = same_as<remove_cv<T>, decltype(nullptr)>;
template<typename T> concept is_cat = included_in<remove_cv<T>, char, wchar_t>;
template<typename T> concept is_uct = included_in<remove_cv<T>, char8_t, char16_t, char32_t>;
template<typename T> concept character = is_cat<T> || is_uct<T>;
template<typename T> concept is_int = included_in<remove_cv<T>, signed char, short, int, long, long long>;
template<typename T> concept is_nat = included_in<remove_cv<T>, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
template<typename T> concept is_fat = included_in<remove_cv<T>, float, double, long double>;
template<typename T> concept scalar = arithmetic<T> || is_pointer<T> || is_nullptr<T> || is_null<T> || is_enum<T>;

template<typename T> using add_lvref = std::add_lvalue_reference_t<T>;
template<typename T> using add_rvref = std::remove_reference_t<T>&&;
template<typename T> using add_fwref = std::add_rvalue_reference_t<T>;

inline constexpr auto mv = []<typename T>(T&& Ref) noexcept -> add_rvref<T> { return static_cast<add_rvref<T>>(Ref); };
template<typename T> inline constexpr auto fwd = []<typename U>(U&& Ref) noexcept -> T&& { return static_cast<T&&>(Ref); };
template<typename T> inline constexpr auto declval = []() noexcept -> T&& {};

template<typename T> inline constexpr auto construct =
[]<typename... As>(As&&... Args) noexcept(noexcept(T{fwd<As>(Args)...})) -> T
  requires requires { T{fwd<As>(Args)...}; } { return T{fwd<As>(Args)...}; };
template<typename T, typename... As> concept constructible_from = requires { construct<T>(declval<As>()...); };
template<typename T, typename... As> concept nt_constructible = noexcept(construct<T>(declval<As>()...));

inline constexpr auto assign =
[]<typename T, typename U>(T& Ref, U&& Val) noexcept(noexcept(Ref = fwd<U>(Val))) -> T&
  requires requires { Ref = fwd<U>(Val); } { Ref = fwd<U>(Val); return Ref; };
template<typename T, typename U> concept assignable = requires { assign(declval<T>(), declval<U>()); };
template<typename T, typename U> concept nt_assignable = noexcept(assign(declval<T>(), declval<U>()));

inline constexpr auto exchange =
[]<typename T, typename U>(T& Ref, U&& Val) noexcept(nt_constructible<T, T> && nt_assignable<T&, U>)
  requires constructible_from<T, U> && assignable<T&, U> { T Old = mv(Ref); Ref = fwd<U>(Val); return Old; };
template<typename T, typename U> concept exchangeable = requires { exchange(declval<T>(), declval<U>()); };
template<typename T, typename U> concept nt_exchangeable = noexcept(exchange(declval<T>(), declval<U>()));

template<typename T> inline constexpr auto bitcast =
[]<typename U>(const U& Val) noexcept -> T { return std::bit_cast<T>(Val); };
}
namespace std {
template<typename T> struct common_type<yw::null_t, T> : common_type<double, T> {};
template<typename T> struct common_type<T, yw::null_t> : common_type<T, double> {};
}


namespace yw { // tuples

namespace get_strategy {
enum class strategy { invalid = 0, itself = 0x1, array = 0x2, tuple = 0x4, member = 0x8, nothrow = 0x10 };
constexpr strategy operator|(strategy a, strategy b) { return strategy(int(a) | int(b)); }
constexpr strategy operator&(strategy a, strategy b) { return strategy(int(a) & int(b)); }
constexpr strategy operator^(strategy a, strategy b) { return strategy(int(a) ^ int(b)); }
constexpr strategy operator~(strategy a) { return strategy(~int(a)); }
using enum strategy;
template<int I> void get() = delete;
template<typename T, int I> inline constexpr strategy check = []() -> strategy {
  using t = remove_ref<T>;
  if constexpr (is_bounded_array<t>) return I < std::extent_v<t> ? array | nothrow : invalid;
  else if constexpr (is_unbounded_array<t>) return invalid;
  else if constexpr (is_class<t> || is_union<t>) {
    if constexpr (requires { declval<T>().template get<I>(); })
      return noexcept(declval<T>().template get<I>()) ? member | tuple | nothrow : member | tuple;
    else if constexpr (requires { get<I>(declval<T>()); }) return noexcept(get<I>(declval<T>())) ? tuple | nothrow : tuple;
    else return I == 0 ? itself | nothrow : invalid;
  } else return I == 0 ? itself | nothrow : invalid;
}();
template<int I, typename T> requires (check<T, I> != strategy::invalid)
decltype(auto) call(T&& Ref) noexcept(bool(check<T, I> & strategy::nothrow)) {
  constexpr auto s = check<T, I>;
  if constexpr (bool(s & strategy::itself)) return fwd<T>(Ref);
  else if constexpr (bool(s & strategy::array)) return fwd<T>(Ref)[I];
  else if constexpr (bool(s & strategy::member)) return fwd<T>(Ref).template get<I>();
  else return get<I>(fwd<T>(Ref));
}
}
template<typename T> concept tuple = bool(get_strategy::check<T, 0> & (get_strategy::tuple | get_strategy::array));
template<typename T, int I> concept gettable = get_strategy::check<T, I> != get_strategy::invalid;
template<typename T, int I> concept nt_gettable = gettable<T, I> && bool(get_strategy::check<T, I> & get_strategy::nothrow);
template<int I> inline constexpr auto get = []<typename T>(T&& Ref) ywlib_wrap_ref(get_strategy::call<I>(fwd<T>(Ref)));
template<typename T, int I> requires gettable<T, I> using element_t = decltype(get<I>(declval<T>()));

template<typename T> struct t_extent : constant<0> {};
template<typename T> requires (bool(get_strategy::check<T, 0>& get_strategy::itself)) struct t_extent<T> : constant<1> {};
template<typename T> requires (bool(get_strategy::check<T, 0>& get_strategy::array)) struct t_extent<T> : std::extent<remove_ref<T>> {};
template<typename T> requires (bool(get_strategy::check<T, 0>& get_strategy::tuple)) struct t_extent<T> : std::tuple_size<remove_ref<T>> {};
template<typename T> inline constexpr int extent = int(t_extent<T>::value);
template<typename T, typename... Ts> concept same_extent = ((extent<T> == extent<Ts>) && ...);
}

namespace yw { // ranges

template<typename T> concept iterator = std::input_or_output_iterator<remove_ref<T>>;
template<typename T> concept input_iterator = std::input_iterator<remove_ref<T>>;
template<typename T, typename In> concept output_iterator = std::output_iterator<remove_ref<T>, In>;
template<typename T> concept forward_iterator = std::forward_iterator<remove_ref<T>>;
template<typename T> concept bidirectional_iterator = std::bidirectional_iterator<remove_ref<T>>;
template<typename T> concept random_access_iterator = std::random_access_iterator<remove_ref<T>>;
template<typename T> concept contiguous_iterator = std::contiguous_iterator<remove_ref<T>>;

template<typename T> concept range = std::ranges::range<T>;
template<typename T> concept input_range = range<T> && std::ranges::input_range<T>;
template<typename T, typename In> concept output_range = range<T> && std::ranges::output_range<T, In>;
template<typename T> concept forward_range = range<T> && std::ranges::forward_range<T>;
template<typename T> concept bidirectional_range = range<T> && std::ranges::bidirectional_range<T>;
template<typename T> concept random_access_range = range<T> && std::ranges::random_access_range<T>;
template<typename T> concept contiguous_range = range<T> && std::ranges::contiguous_range<T>;

template<range Rg> using iterator_t = std::ranges::iterator_t<Rg>;
template<range Rg> using sentinel_t = std::ranges::sentinel_t<Rg>;

template<typename T> struct t_iter_type;
template<iterator T> struct t_iter_type<T> {
  using value_t = std::iter_value_t<T>;
  using reference_t = std::iter_reference_t<T>;
  using rvalue_reference_t = std::iter_rvalue_reference_t<T>;
  using difference_t = std::iter_difference_t<T>;
};
template<range T> struct t_iter_type<T> : t_iter_type<iterator_t<T>> {};
template<typename T> requires (iterator<T> || range<T>) using iter_value_t = typename t_iter_type<remove_ref<T>>::value_t;
template<typename T> requires (iterator<T> || range<T>) using iter_reference_t = typename t_iter_type<remove_ref<T>>::reference_t;
template<typename T> requires (iterator<T> || range<T>) using iter_rvalue_reference_t = typename t_iter_type<remove_ref<T>>::rvalue_reference_t;
template<typename T> requires (iterator<T> || range<T>) using iter_difference_t = typename t_iter_type<remove_ref<T>>::difference_t;

inline constexpr auto begin = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::begin(fwd<Rg>(r)));
inline constexpr auto end = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::end(fwd<Rg>(r)));
inline constexpr auto rbegin = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::rbegin(fwd<Rg>(r)));
inline constexpr auto rend = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::rend(fwd<Rg>(r)));
inline constexpr auto size = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::size(fwd<Rg>(r)));
inline constexpr auto empty = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::empty(fwd<Rg>(r)));
inline constexpr auto data = []<range Rg>(Rg&& r) ywlib_wrap_auto(std::ranges::data(fwd<Rg>(r)));
inline constexpr auto iter_move = []<iterator It>(It&& i) ywlib_wrap_ref(std::ranges::iter_move(fwd<It>(i)));
inline constexpr auto iter_swap = []<iterator It, iterator Jt>(It&& i, Jt&& j) ywlib_wrap_void(std::ranges::iter_swap(fwd<It>(i), fwd<Jt>(j)));
}

namespace yw { // array<T, N>

template<typename T, int N = npos> requires (N >= -1) class array {
public:
  static constexpr int count = N;
  T _[N];
  constexpr bool empty() const noexcept { return false; }
  constexpr int size() const noexcept { return N; }
  constexpr T* data() noexcept { return _; }
  constexpr const T* data() const noexcept { return _; }
  constexpr T& operator[](int i) noexcept { return _[i]; }
  constexpr const T& operator[](int i) const noexcept { return _[i]; }
  constexpr T* begin() noexcept { return _; }
  constexpr const T* begin() const noexcept { return _; }
  constexpr T* end() noexcept { return _ + N; }
  constexpr const T* end() const noexcept { return _ + N; }
  template<int I> requires (I < N) constexpr T& get() & noexcept { return _[I]; }
  template<int I> requires (I < N) constexpr const T& get() const & noexcept { return _[I]; }
  template<int I> requires (I < N) constexpr T&& get() && noexcept { return mv(_[I]); }
  template<int I> requires (I < N) constexpr const T&& get() const && noexcept { return mv(_[I]); }
};

template<typename T> class array<T, 0> {
public:
  static constexpr int count = 0;
  constexpr bool empty() const noexcept { return true; }
  constexpr int size() const noexcept { return 0; }
  constexpr T* data() noexcept { return nullptr; }
  constexpr const T* data() const noexcept { return nullptr; }
  constexpr T* begin() noexcept { return nullptr; }
  constexpr const T* begin() const noexcept { return nullptr; }
  constexpr T* end() noexcept { return nullptr; }
  constexpr const T* end() const noexcept { return nullptr; }
};

template<typename T> class array<T, npos> {
  int _count{};
  T* _ptr{};
public:
  constexpr ~array() noexcept { if (_ptr) delete[] _ptr; }
  constexpr array() noexcept = default;
  constexpr array(array&& a) noexcept : _count(a._count), _ptr(exchange(a._ptr, nullptr)) {}
  constexpr array& operator=(array&& a) {
    if (_ptr) delete[] _ptr;
    _count = a._count, _ptr = exchange(a._ptr, nullptr);
    return *this;
  }
  constexpr array(int count) : _count(count), _ptr(new T[count]) {}
  constexpr array(int count, const T& value) : array(count) { for (int i = 0; i < count; ++i) _ptr[i] = value; }
  template<input_range Rg> requires assignable<T&, iter_reference_t<Rg>> constexpr array(Rg&& r)
    requires different_from<Rg, array> : array(std::ranges::size(r)) { std::ranges::copy(r, begin()); }
  template<input_range Rg> requires assignable<T&, iter_reference_t<Rg>> constexpr array& operator=(Rg&& r)
    requires different_from<Rg, array> { return *this = array(r); }
  constexpr bool empty() const noexcept { return !_count; }
  constexpr int size() const noexcept { return _count; }
  constexpr T* data() noexcept { return _ptr; }
  constexpr const T* data() const noexcept { return _ptr; }
  constexpr T& operator[](int i) noexcept { return _ptr[i]; }
  constexpr const T& operator[](int i) const noexcept { return _ptr[i]; }
  constexpr T* begin() noexcept { return _ptr; }
  constexpr const T* begin() const noexcept { return _ptr; }
  constexpr T* end() noexcept { return _ptr + _count; }
  constexpr const T* end() const noexcept { return _ptr + _count; }
};

template<typename T, typename... Ts> array(Ts&&...) -> array<T, 1 + sizeof...(Ts)>;
template<input_range Rg> array(Rg&&) -> array<iter_value_t<Rg>>;
template<typename T> array(int n, const T&) -> array<T>;
}
namespace std {
template<typename T, int N> requires (N >= 0) struct tuple_size<yw::array<T, N>> : integral_constant<int, N> {};
template<size_t I, typename T, int N> requires (N >= 0) struct tuple_element<I, yw::array<T, N>> : type_identity<T> {};
}


namespace yw { // string

template<typename T, typename Ct = iter_value_t<T>> concept stringable = nt_convertible_to<T, std::basic_string_view<Ct>>;

inline constexpr auto strlen = []<stringable Str>(Str&& s) -> int {
  if constexpr (is_bounded_array<remove_ref<Str>>) return extent<Str> - 1;
  else if constexpr (is_unbounded_array<remove_ref<Str>> || is_pointer<remove_ref<Str>>) return int(std::char_traits<iter_value_t<Str>>::length(s));
  else if constexpr (same_as<remove_cvref<Str>, std::basic_string<iter_value_t<Str>>>) return int(s.size());
  else if constexpr (same_as<remove_cvref<Str>, std::basic_string_view<iter_value_t<Str>>>) return int(s.size());
  else return int(std::basic_string_view<iter_value_t<Str>>(s).size());
};

struct string_view {
  const char* const _ptr{};
  const int _count{};
  constexpr string_view(const char* Ptr, int Count) : _ptr(Ptr), _count(Count) {}
  constexpr string_view(const char* Ptr) : _ptr(Ptr), _count(strlen(Ptr)) {}
  constexpr string_view(const std::string_view& s) : _ptr(s.data()), _count(int(s.size())) {}
  constexpr bool empty() const noexcept { return !_count; }
  constexpr int size() const noexcept { return _count; }
  constexpr const char* data() const noexcept { return _ptr; }
  constexpr char operator[](int i) const noexcept { return _ptr[i]; }
  constexpr const char* begin() const noexcept { return _ptr; }
  constexpr const char* end() const noexcept { return _ptr + _count; }
  constexpr string_view substr(int pos, int count) const {
    if (count == npos) count = _count - pos;
    return string_view(_ptr + pos, count);
  }
  constexpr string_view substr(int count) const { return string_view(_ptr, count); }
};
}
namespace std {
template<> struct formatter<yw::string_view> : std::formatter<string_view> {
  auto format(yw::string_view s, auto& ctx) const { return std::formatter<string_view>::format({s.data(), size_t(s.size())}, ctx); }
};
}

// class string {
//   int _count{};
//   cat* _ptr{};
//   static constexpr cat* cvt(const char* s, int n, cat* out) {
//     for (int i = 0; i < n;) {
//       auto c = s[i++];
//       if (c < 0x80) *out++ = c;
//       else if (c < 0xE0) *out++ = ((c & 0x1F) << 6) | (s[i++] & 0x3F);
//       else if (c < 0xF0) *out++ = ((c & 0x0F) << 12) | ((s[i++] & 0x3F) << 6) | (s[i++] & 0x3F);
//       else *out++ = ((c & 0x07) << 18) | ((s[i++] & 0x3F) << 12) | ((s[i++] & 0x3F) << 6) | (s[i++] & 0x3F);
//     }
//     return out;
//   }
//   static constexpr cat* cvt(const wchar_t* s, int n, cat* out) {
//     for (int i = 0; i < n;) {
//       auto c = s[i++];
//       if (c < 0xD800 || c >= 0xDC00) *out++ = c;
//       else *out++ = 0x10000 + ((c & 0x3FF) << 10) + (s[i++] & 0x3FF);
//     }
//     return out;
//   }
// public:
//   constexpr ~string() noexcept { if (_ptr) delete[] _ptr; }
//   constexpr string() noexcept = default;
//   constexpr string(string&& s) noexcept : _count(s._count), _ptr(exchange(s._ptr, nullptr)) {}
//   constexpr string& operator=(string&& s) {
//     if (_ptr) delete[] _ptr;
//     _count = s._count, _ptr = exchange(s._ptr, nullptr);
//     return *this;
//   }
//   constexpr string(int Count, cat Fill) : _count(Count), _ptr(new cat[Count + 1]) {
//     *std::ranges::fill_n(_ptr, Count, Fill) = 0;
//   }
//   constexpr string(const cat* Ptr, int Count) : _count(Count), _ptr(new cat[Count + 1]) {
//     *std::ranges::copy_n(Ptr, Count, _ptr).out = 0;
//   }
//   constexpr string(std::basic_string_view<char> s) {
//     _ptr = new cat[s.size() + 1];
//     auto se = cvt(s.data(), s.size(), _ptr);
//     *se = 0, _count = se - _ptr;
//   }
//   constexpr string(std::basic_string_view<wchar_t> s) {
//     _ptr = new cat[s.size() + 1];
//     auto se = cvt(s.data(), s.size(), _ptr);
//     *se = 0, _count = se - _ptr;
//   }
//   constexpr string(std::basic_string_view<char8_t> s) : string(bitcast<std::basic_string_view<char>>(s)) {}
//   constexpr string(std::basic_string_view<char16_t> s) : string(bitcast<std::basic_string_view<wchar_t>>(s)) {}
//   constexpr string(std::basic_string_view<char32_t> s) : string(s.data(), s.size()) {}
//   constexpr string& operator=(std::basic_string_view<char> s) {
//     if (_ptr) delete[] _ptr;
//     _ptr = new cat[s.size() + 1];
//     auto se = cvt(s.data(), s.size(), _ptr);
//     *se = 0, _count = se - _ptr;
//     return *this;
//   }
//   constexpr string& operator=(std::basic_string_view<wchar_t> s) {
//     if (_ptr) delete[] _ptr;
//     _ptr = new cat[s.size() + 1];
//     auto se = cvt(s.data(), s.size(), _ptr);
//     *se = 0, _count = se - _ptr;
//     return *this;
//   }
//   constexpr string& operator=(std::basic_string_view<char8_t> s) { return *this = bitcast<std::basic_string_view<char>>(s); }
//   constexpr string& operator=(std::basic_string_view<char16_t> s) { return *this = bitcast<std::basic_string_view<wchar_t>>(s); }
//   constexpr string& operator=(std::basic_string_view<char32_t> s) {
//     if (_ptr) delete[] _ptr;
//     _ptr = new cat[s.size() + 1];
//     *std::ranges::copy_n(s.data(), s.size(), _ptr).out = 0;
//     return *this;
//   }
//   constexpr bool empty() const noexcept { return !_count; }
//   constexpr int size() const noexcept { return _count; }
//   constexpr cat* data() noexcept { return _ptr; }
//   constexpr const cat* data() const noexcept { return _ptr; }
//   constexpr cat& operator[](int i) noexcept { return _ptr[i]; }
//   constexpr const cat& operator[](int i) const noexcept { return _ptr[i]; }
//   constexpr cat* begin() noexcept { return _ptr; }
//   constexpr const cat* begin() const noexcept { return _ptr; }
//   constexpr cat* end() noexcept { return _ptr + _count; }
//   constexpr const cat* end() const noexcept { return _ptr + _count; }
//   constexpr string substr(int pos, int count) const {
//     if (count == npos) count = _count - pos;
//     return string(_ptr + pos, count);
//   }
//   constexpr string substr(int count) const { return string(_ptr, count); }
//   constexpr bool starts_with(const string& s) const {
//     return _count >= s._count && std::ranges::equal(s, *this);
//   }
// };


// }

// namespace yw {



// using string = std::u32string;
// using string_view = std::u32string_view;
// using namespace std::string_literals;

// constexpr std::wstring to_wstring(const string& s) {
//   std::wstring r(s.size() * 2, 0);
//   auto p = r.begin();
//   for (const auto c : s) {
//     if (c >= 0x10000) {
//       *p++ = static_cast<wchar_t>(0xD800 + ((c - 0x10000) >> 10));
//       *p++ = static_cast<wchar_t>(0xDC00 + ((c - 0x10000) & 0x3FF));
//     } else *p++ = static_cast<wchar_t>(c);
//   }
//   r.resize(p - r.begin());
//   return r;
// }

// constexpr std::string to_string(const string& s) {
//   std::string r(s.size() * 4, 0);
//   auto p = r.begin();
//   for (const auto c : s) {
//     if (c >= 0x10000) {
//       *p++ = static_cast<char>(0xF0 | ((c >> 18) & 0x07));
//       *p++ = static_cast<char>(0x80 | ((c >> 12) & 0x3F));
//       *p++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
//       *p++ = static_cast<char>(0x80 | (c & 0x3F));
//     } else if (c >= 0x800) {
//       *p++ = static_cast<char>(0xE0 | ((c >> 12) & 0x0F));
//       *p++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
//       *p++ = static_cast<char>(0x80 | (c & 0x3F));
//     } else if (c >= 0x80) {
//       *p++ = static_cast<char>(0xC0 | ((c >> 6) & 0x1F));
//       *p++ = static_cast<char>(0x80 | (c & 0x3F));
//     } else *p++ = static_cast<char>(c);
//   }
//   r.resize(p - r.begin());
//   return r;
// }

// struct null_t {
//   constexpr null_t() noexcept = default;
//   constexpr null_t(auto&&...) noexcept {}
//   constexpr null_t& operator=(auto&&) noexcept { return *this; }
//   constexpr const null_t& operator=(auto&&) const noexcept { return *this; }
//   explicit constexpr operator bool() const noexcept { return false; }
//   friend constexpr bool operator==(null_t, null_t) noexcept { return false; }
//   friend constexpr auto operator<=>(null_t, null_t) noexcept { return std::partial_ordering::unordered; }
//   friend constexpr null_t operator+(null_t) noexcept { return {}; }
//   friend constexpr null_t operator-(null_t) noexcept { return {}; }
//   friend constexpr null_t operator+(null_t, null_t) noexcept { return {}; }
//   friend constexpr null_t operator-(null_t, null_t) noexcept { return {}; }
//   friend constexpr null_t operator*(null_t, null_t) noexcept { return {}; }
//   friend constexpr null_t operator/(null_t, null_t) noexcept { return {}; }
//   friend constexpr null_t& operator+=(null_t& n, null_t) noexcept { return n; }
//   friend constexpr null_t& operator-=(null_t& n, null_t) noexcept { return n; }
//   friend constexpr null_t& operator*=(null_t& n, null_t) noexcept { return n; }
//   friend constexpr null_t& operator/=(null_t& n, null_t) noexcept { return n; }
// };

// inline constexpr null_t null{};

// template<typename T> class nullable {
// public:
//   bool has_value;
//   T value{};

//   constexpr nullable() : has_value(false) {}
//   constexpr nullable(const T& value) : has_value(true), value(value) {}
//   constexpr nullable(T&& value) : has_value(true), value(static_cast<T&&>(value)) {}

//   constexpr operator bool() const {
//     if constexpr (requires { static_cast<bool>(value); }) return has_value && bool(value);
//     else return has_value;
//   }
//   constexpr bool is_null() const { return !has_value; }
//   constexpr T& get() { return value; }
//   constexpr const T& get() const { return value; }
// };

// struct vector2 {
//   double x, y;
// };

// inline const HINSTANCE hinstance = GetModuleHandleW(nullptr);

// inline bool ok(const string& text, const string& caption = U"OK?"s) {
//   auto text_w = to_wstring(text), caption_w = to_wstring(caption);
//   return MessageBoxW(nullptr, text_w.c_str(), caption_w.c_str(), MB_OK) == IDOK;
// }

// inline bool yesno(const string& text, const string& caption = U"YES?"s) {
//   auto text_w = to_wstring(text), caption_w = to_wstring(caption);
//   return MessageBoxW(nullptr, text_w.c_str(), caption_w.c_str(), MB_YESNO) == IDYES;
// }

// class window;

// ////////////////////////////////////////////////////////////////////////////////
// //  WINDOW MESSAGE

// enum class wmessage : unsigned {
//   wm_null = WM_NULL, wm_create = WM_CREATE, wm_destroy = WM_DESTROY, wm_move = WM_MOVE, wm_size = WM_SIZE, wm_activate = WM_ACTIVATE,
//   wm_setfocus = WM_SETFOCUS, wm_killfocus = WM_KILLFOCUS, wm_enable = WM_ENABLE, wm_setredraw = WM_SETREDRAW, wm_settext = WM_SETTEXT, wm_gettext = WM_GETTEXT,
//   wm_gettextlength = WM_GETTEXTLENGTH, wm_paint = WM_PAINT, wm_close = WM_CLOSE, wm_queryendsession = WM_QUERYENDSESSION, wm_queryopen = WM_QUERYOPEN,
//   wm_endsession = WM_ENDSESSION, wm_quit = WM_QUIT, wm_erasebkgnd = WM_ERASEBKGND, wm_syscolorchange = WM_SYSCOLORCHANGE, wm_showwindow = WM_SHOWWINDOW,
//   wm_wininichange = WM_WININICHANGE, wm_settingchange = WM_SETTINGCHANGE, wm_devmodechange = WM_DEVMODECHANGE, wm_activateapp = WM_ACTIVATEAPP,
//   wm_fontchange = WM_FONTCHANGE, wm_timechange = WM_TIMECHANGE, wm_cancelsmode = WM_CANCELMODE, wm_setcursor = WM_SETCURSOR,
//   wm_mouseactivate = WM_MOUSEACTIVATE, wm_childactivate = WM_CHILDACTIVATE, wm_queuesync = WM_QUEUESYNC, wm_getminmaxinfo = WM_GETMINMAXINFO,
//   wm_painticon = WM_PAINTICON, wm_iconerasebkgnd = WM_ICONERASEBKGND, wm_nextdlgctl = WM_NEXTDLGCTL, wm_spoolerstatus = WM_SPOOLERSTATUS,
//   wm_drawitem = WM_DRAWITEM,  wm_measureitem = WM_MEASUREITEM, wm_deleteitem = WM_DELETEITEM, wm_vkeytoitem = WM_VKEYTOITEM, wm_chartoitem = WM_CHARTOITEM,
//   wm_setfont = WM_SETFONT, wm_getfont = WM_GETFONT, wm_sethotkey = WM_SETHOTKEY, wm_gethotkey = WM_GETHOTKEY, wm_querydragicon = WM_QUERYDRAGICON,
//   wm_compareitem = WM_COMPAREITEM, wm_getobject = WM_GETOBJECT, wm_compacting = WM_COMPACTING, wm_commnotify = WM_COMMNOTIFY,
//   wm_windowposchanging = WM_WINDOWPOSCHANGING, wm_windowposchanged = WM_WINDOWPOSCHANGED, wm_power = WM_POWER, wm_copydata = WM_COPYDATA,
//   wm_canceljournal = WM_CANCELJOURNAL, wm_notify = WM_NOTIFY, wm_inputlangchangerequest = WM_INPUTLANGCHANGEREQUEST, wm_inputlangchange = WM_INPUTLANGCHANGE,
//   wm_tcard = WM_TCARD, wm_help = WM_HELP, wm_userchanged = WM_USERCHANGED, wm_notifyformat = WM_NOTIFYFORMAT, wm_contextmenu = WM_CONTEXTMENU,
//   wm_stylechanging = WM_STYLECHANGING, wm_stylechanged = WM_STYLECHANGED, wm_displaychange = WM_DISPLAYCHANGE, wm_geticon = WM_GETICON,
//   wm_seticon = WM_SETICON, wm_nccreate = WM_NCCREATE, wm_ncdestroy = WM_NCDESTROY, wm_nccalcsize = WM_NCCALCSIZE, wm_nchittest = WM_NCHITTEST,
//   wm_ncpaint = WM_NCPAINT, wm_ncactivate = WM_NCACTIVATE, wm_getdlgcode = WM_GETDLGCODE, wm_syncpaint = WM_SYNCPAINT, wm_ncmousemove = WM_NCMOUSEMOVE,
//   wm_nclbuttondown = WM_NCLBUTTONDOWN, wm_nclbuttonup = WM_NCLBUTTONUP, wm_nclbuttondblclk = WM_NCLBUTTONDBLCLK, wm_ncrbuttondown = WM_NCRBUTTONDOWN,
//   wm_ncrbuttonup = WM_NCRBUTTONUP, wm_ncrbuttondblclk = WM_NCRBUTTONDBLCLK, wm_ncmbuttondown = WM_NCMBUTTONDOWN, wm_ncmbuttonup = WM_NCMBUTTONUP,
//   wm_ncmbuttondblclk = WM_NCMBUTTONDBLCLK, wm_ncxbuttondown = WM_NCXBUTTONDOWN, wm_ncxbuttonup = WM_NCXBUTTONUP, wm_ncxbuttondblclk = WM_NCXBUTTONDBLCLK,
//   wm_input_device_change = WM_INPUT_DEVICE_CHANGE, wm_input = WM_INPUT, wm_keyfirst = WM_KEYFIRST, wm_keydown = WM_KEYDOWN, wm_keyup = WM_KEYUP,
//   wm_char = WM_CHAR, wm_deadchar = WM_DEADCHAR, wm_syskeydown = WM_SYSKEYDOWN, wm_syskeyup = WM_SYSKEYUP, wm_syschar = WM_SYSCHAR,
//   wm_sysdeadchar = WM_SYSDEADCHAR, wm_unichar = WM_UNICHAR, wm_keylast = WM_KEYLAST, wm_ime_startcomposition = WM_IME_STARTCOMPOSITION,
//   wm_ime_endcomposition = WM_IME_ENDCOMPOSITION, wm_ime_composition = WM_IME_COMPOSITION, wm_ime_keylast = WM_IME_KEYLAST, wm_initdialog = WM_INITDIALOG,
//   wm_command = WM_COMMAND, wm_syscommand = WM_SYSCOMMAND, wm_timer = WM_TIMER, wm_hscroll = WM_HSCROLL, wm_vscroll = WM_VSCROLL, wm_initmenu = WM_INITMENU,
//   wm_initmenupopup = WM_INITMENUPOPUP, wm_gesture = WM_GESTURE, wm_gesturenotify = WM_GESTURENOTIFY, wm_menuselect = WM_MENUSELECT, wm_menuchar = WM_MENUCHAR,
//   wm_enteridle = WM_ENTERIDLE, wm_menurbuttonup = WM_MENURBUTTONUP, wm_menudrag = WM_MENUDRAG, wm_menugetobject = WM_MENUGETOBJECT,
//   wm_uninitmenupopup = WM_UNINITMENUPOPUP, wm_menucommand = WM_MENUCOMMAND, wm_changeuistate = WM_CHANGEUISTATE, wm_updateuistate = WM_UPDATEUISTATE,
//   wm_queryuistate = WM_QUERYUISTATE, wm_ctlcolormsgbox = WM_CTLCOLORMSGBOX, wm_ctlcoloredit = WM_CTLCOLOREDIT, wm_ctlcolorlistbox = WM_CTLCOLORLISTBOX,
//   wm_ctlcolorbtn = WM_CTLCOLORBTN, wm_ctlcolordlg = WM_CTLCOLORDLG, wm_ctlcolorscrollbar = WM_CTLCOLORSCROLLBAR, wm_ctlcolorstatic = WM_CTLCOLORSTATIC,
//   mn_gethmenu = MN_GETHMENU, wm_mousefirst = WM_MOUSEFIRST, wm_mousemove = WM_MOUSEMOVE, wm_lbuttondown = WM_LBUTTONDOWN, wm_lbuttonup = WM_LBUTTONUP,
//   wm_lbuttondblclk = WM_LBUTTONDBLCLK, wm_rbuttondown = WM_RBUTTONDOWN, wm_rbuttonup = WM_RBUTTONUP, wm_rbuttondblclk = WM_RBUTTONDBLCLK,
//   wm_mbuttondown = WM_MBUTTONDOWN, wm_mbuttonup = WM_MBUTTONUP, wm_mbuttondblclk = WM_MBUTTONDBLCLK, wm_mousewheel = WM_MOUSEWHEEL,
//   wm_xbuttondown = WM_XBUTTONDOWN, wm_xbuttonup = WM_XBUTTONUP, wm_xbuttondblclk = WM_XBUTTONDBLCLK, wm_mousehwheel = WM_MOUSEHWHEEL,
//   wm_mouselast = WM_MOUSELAST, wm_parentnotify = WM_PARENTNOTIFY, wm_entermenuloop = WM_ENTERMENULOOP, wm_exitmenuloop = WM_EXITMENULOOP,
//   wm_nextmenu = WM_NEXTMENU, wm_sizing = WM_SIZING, wm_capturechanged = WM_CAPTURECHANGED, wm_moving = WM_MOVING, wm_powerbroadcast = WM_POWERBROADCAST,
//   wm_devicechange = WM_DEVICECHANGE, wm_mdicreate = WM_MDICREATE, wm_mdidestroy = WM_MDIDESTROY, wm_mdiactivate = WM_MDIACTIVATE,
//   wm_mdirestore = WM_MDIRESTORE, wm_mdinext = WM_MDINEXT, wm_mdimaximize = WM_MDIMAXIMIZE, wm_mditile = WM_MDITILE, wm_mdicascade = WM_MDICASCADE,
//   wm_mdiiconarrange = WM_MDIICONARRANGE, wm_mdigetactive = WM_MDIGETACTIVE, wm_mdisetmenu = WM_MDISETMENU, wm_entersizemove = WM_ENTERSIZEMOVE,
//   wm_exitsizemove = WM_EXITSIZEMOVE, wm_dropfiles = WM_DROPFILES, wm_mdirefreshmenu = WM_MDIREFRESHMENU, wm_pointerdevicechange = WM_POINTERDEVICECHANGE,
//   wm_pointerdeviceinrange = WM_POINTERDEVICEINRANGE, wm_pointerdeviceoutofrange = WM_POINTERDEVICEOUTOFRANGE, wm_touch = WM_TOUCH,
//   wm_ncpointerupdate = WM_NCPOINTERUPDATE, wm_ncpointerdown = WM_NCPOINTERDOWN, wm_ncpointerup = WM_NCPOINTERUP, wm_pointerupdate = WM_POINTERUPDATE,
//   wm_pointerdown = WM_POINTERDOWN, wm_pointerup = WM_POINTERUP, wm_pointerenter = WM_POINTERENTER, wm_pointerleave = WM_POINTERLEAVE,
//   wm_pointeractivate = WM_POINTERACTIVATE, wm_pointercapturechanged = WM_POINTERCAPTURECHANGED, wm_touchhittesting = WM_TOUCHHITTESTING,
//   wm_pointerwheel = WM_POINTERWHEEL, wm_pointerhwheel = WM_POINTERHWHEEL, dm_pointerhittest = DM_POINTERHITTEST, wm_pointerroutedto = WM_POINTERROUTEDTO,
//   wm_pointerroutedaway = WM_POINTERROUTEDAWAY, wm_pointerroutedreleased = WM_POINTERROUTEDRELEASED, wm_ime_setcontext = WM_IME_SETCONTEXT,
//   wm_ime_notify = WM_IME_NOTIFY, wm_ime_control = WM_IME_CONTROL, wm_ime_compositionfull = WM_IME_COMPOSITIONFULL, wm_ime_select = WM_IME_SELECT,
//   wm_ime_char = WM_IME_CHAR, wm_ime_request = WM_IME_REQUEST, wm_ime_keydown = WM_IME_KEYDOWN, wm_ime_keyup = WM_IME_KEYUP, wm_mousehover = WM_MOUSEHOVER,
//   wm_mouseleave = WM_MOUSELEAVE, wm_ncmousehover = WM_NCMOUSEHOVER, wm_ncmouseleave = WM_NCMOUSELEAVE, wm_wtssession_change = WM_WTSSESSION_CHANGE,
//   wm_tablet_first = WM_TABLET_FIRST, wm_tablet_last = WM_TABLET_LAST, wm_dpichanged = WM_DPICHANGED, wm_dpichanged_beforeparent = WM_DPICHANGED_BEFOREPARENT,
//   wm_dpichanged_afterparent = WM_DPICHANGED_AFTERPARENT, wm_getdpiscaledsize = WM_GETDPISCALEDSIZE, wm_cut = WM_CUT, wm_copy = WM_COPY, wm_paste = WM_PASTE,
//   wm_clear = WM_CLEAR, wm_undo = WM_UNDO, wm_renderformat = WM_RENDERFORMAT, wm_renderallformats = WM_RENDERALLFORMATS,
//   wm_destroyclipboard = WM_DESTROYCLIPBOARD, wm_drawclipboard = WM_DRAWCLIPBOARD, wm_paintclipboard = WM_PAINTCLIPBOARD,
//   wm_vscrollclipboard = WM_VSCROLLCLIPBOARD, wm_sizeclipboard = WM_SIZECLIPBOARD, wm_askcbformatname = WM_ASKCBFORMATNAME, wm_changecbchain = WM_CHANGECBCHAIN,
//   wm_hscrollclipboard = WM_HSCROLLCLIPBOARD, wm_querynewpalette = WM_QUERYNEWPALETTE, wm_paletteischanging = WM_PALETTEISCHANGING,
//   wm_palettechanged = WM_PALETTECHANGED, wm_hotkey = WM_HOTKEY, wm_print = WM_PRINT, wm_printclient = WM_PRINTCLIENT, wm_appcommand = WM_APPCOMMAND,
//   wm_themechanged = WM_THEMECHANGED, wm_clipboardupdate = WM_CLIPBOARDUPDATE, wm_dwmcompositionchanged = WM_DWMCOMPOSITIONCHANGED,
//   wm_dwmncrenderingchanged = WM_DWMNCRENDERINGCHANGED, wm_dwmcolorizationcolorchanged = WM_DWMCOLORIZATIONCOLORCHANGED,
//   wm_dwmwindowmaximizedchange = WM_DWMWINDOWMAXIMIZEDCHANGE, wm_dwmsendiconicthumbnail = WM_DWMSENDICONICTHUMBNAIL,
//   wm_dwmsendiconiclivepreviewbitmap = WM_DWMSENDICONICLIVEPREVIEWBITMAP, wm_gettitlebarinfoex = WM_GETTITLEBARINFOEX, wm_handheldfirst = WM_HANDHELDFIRST,
//   wm_handheldlast = WM_HANDHELDLAST, wm_affirst = WM_AFXFIRST, wm_aflast = WM_AFXLAST, wm_penwinfirst = WM_PENWINFIRST, wm_penwinlast = WM_PENWINLAST,
//   wm_app = WM_APP, wm_user = WM_USER,
// };
// using enum wmessage;
// constexpr auto operator|(wmessage a, wmessage b) { return wmessage(unsigned(a) | unsigned(b)); }
// constexpr auto operator&(wmessage a, wmessage b) { return wmessage(unsigned(a) & unsigned(b)); }
// constexpr auto operator^(wmessage a, wmessage b) { return wmessage(unsigned(a) ^ unsigned(b)); }
// constexpr auto operator~(wmessage a) { return wmessage(~unsigned(a)); }

// ////////////////////////////////////////////////////////////////////////////////
// //  DEFAULT WINDOW USER DATA

// struct default_window_userdata {
//   HWND hwnd;
//   int pad_x, pad_y;
//   bool (*userproc)(window&, wmessage, unsigned long long wp, long long lp);
//   MSG msg;
//   array<path> files;
// };

// ////////////////////////////////////////////////////////////////////////////////
// //  DEFAULT WINDOW PROCEDURE

// LRESULT CALLBACK default_window_procedure(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
//   auto userdata = (default_window_userdata*)::GetWindowLongPtrW(hw, GWLP_USERDATA);
//   if (userdata && userdata->userproc)
//     if (userdata->userproc(*reinterpret_cast<window*>(userdata), wmessage(msg), wp, lp)) return 0;
//   return DefWindowProcW(hw, msg, wp, lp);
// }

// ////////////////////////////////////////////////////////////////////////////////
// //  DEFAULT WINDOW CLASS

// inline const wchar_t* const default_window_class = [] {
//   WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
//   wc.lpfnWndProc = default_window_procedure;
//   wc.hInstance = hinstance;
//   wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
//   wc.lpszClassName = L"YW_DEFAULT_WINDOW_CLASS";
//   auto atom = static_cast<unsigned long long>(::RegisterClassExW(&wc));
//   return reinterpret_cast<const wchar_t*>(atom);
// }();

// class window {
//   HWND handle{};
//   int pad_x{}, pad_y{};
//   bool (*userproc)(window&, wmessage, unsigned long long wp, long long lp){};
//   MSG msg{};
//   array<path> files{};
// public:

//   window() : handle(nullptr) {}
//   window(HWND handle) : handle(handle) {}
//   window(const window&) = delete;
//   window(window&& other) : handle(std::exchange(other.handle, nullptr)) {}
//   ~window() { if (handle) DestroyWindow(handle); }

//   window& operator=(const window&) = delete;
//   window& operator=(window&& other) {
//     if (handle) DestroyWindow(handle);
//     handle = std::exchange(other.handle, nullptr);
//     return *this;
//   }
// };

// inline window create_window(
//   const string& title,
//   nullable<vector2> position = {},
//   nullable<vector2> size = {},
//   nullable<bool> visible = true,
//   nullable<bool> frame = true) {
//   auto title_w = to_wstring(title);
//   unsigned long s = visible ? WS_VISIBLE : 0;
//   if (frame) s |= WS_CAPTION | WS_SYSMENU;
//   int x = position ? static_cast<int>(position.value.x) : CW_USEDEFAULT;
//   int y = position ? static_cast<int>(position.value.y) : CW_USEDEFAULT;
//   int w = size ? static_cast<int>(size.value.x) : CW_USEDEFAULT;
//   int h = size ? static_cast<int>(size.value.y) : CW_USEDEFAULT;
//   auto hw = CreateWindowExW(
//     WS_EX_ACCEPTFILES, default_window_class, title_w.c_str(), s,
//     x, y, w, h, nullptr, nullptr, hinstance, nullptr);
//   return window(hw);
// }

// }
