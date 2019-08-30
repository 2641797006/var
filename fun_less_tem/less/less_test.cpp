#include <iostream>

// 2个参数的函数,  记录函数参数,返回值类型, c++17中弃用
template<typename _Arg1, typename _Arg2, typename _Result>
struct binary_function
{
	typedef _Arg1 first_argument_type;
	typedef _Arg2 second_argument_type;
	typedef _Result result_type;
};

template<typename T = void>
struct less;

template<typename T>
struct less : public binary_function<T, T, bool>
{
	constexpr
	bool
	operator()(const T& x, const T& y) const
	{
		return x < y;
	}
};

struct __is_transparent;
template<>
struct less<void>
{
	template <typename T, typename _Up>
	constexpr
	auto
	operator()(T&& t, _Up&& u) const
	noexcept(noexcept(std::forward<T>(t) < std::forward<_Up>(u)))
	-> decltype(std::forward<T>(t) < std::forward<_Up>(u))
	{
		return std::forward<T>(t) < std::forward<_Up>(u);
	}
	typedef __is_transparent is_transparent;
};


template <typename A, typename B, typename U = less<>>
bool f(A a, B b, U u = U())
{
    return u(a, b);
}
 
int main() 
{
    std::cout << std::boolalpha;   
    std::cout << f(5, 20) << '\n';
    std::cout << f(100, 10) << '\n';
}
