// oper.h - C++ wrapper for XLOPER struct
#pragma once
#include <concepts>
#include "xll.h"

namespace xll {

	// null terminated array length
	template<class T>
	constexpr size_t len(const T* p, size_t n = 0)
	{
		return *p ? len(p + 1, n + 1) : n;
	}

	template<class X>
	struct oper_traits {
	};

	template<>
	struct oper_traits<XLOPER> {
		using xchar = CHAR;
	};

	template<>
	struct oper_traits<XLOPER12> {
		using xchar = XCHAR;
	};

	template<class X>
		requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	struct XOPER : public X {
		using X::xltype;
		using X::val;

		// Type without allocation bits.
		constexpr DWORD type() const
		{
			return xltype & (~xlbitDLLFree | ~xlbitXLFree);
		}

		// Default constructor.
		constexpr XOPER()
		{
			xltype = xltypeNil;
		}

		// 64-bit double precison floating point
		constexpr explicit XOPER(double num) noexcept
		{
			xltype = xltypeNum;
			val.num = num;
		}
		XOPER& operator=(double num)
		{
			deallocate();

			return *this = XOPER(num);
		}
		constexpr bool operator==(double num) const
		{
			return type() == xltypeNum && val.num == num;
		}

		// counted wide string
		XOPER(oper_traits<X>::xchar const* str, size_t n)
		{
			ensure(n <= std::numeric_limits<XCHAR>::max()/2);
			xltype = xltypeStr;
			val.str[0] = static_cast<XCHAR>(n);
			str = new oper_traits<X>::xchar[](1 + val.str[0]);
			wcscpy_s(1 + val.str, n, str);
		}
		explicit XOPER(oper_traits<X>::xchar const* str)
			: XOPER(str, len(str))
		{ }
		XOPER& operator=(oper_traits<X>::xchar const* str)
		{
			deallocate();

			return *this = XOPER(str);
		}
		bool operator==(XCHAR const* str) const
		{
			size_t n = wcslen(str);

			return n <= std::numeric_limits<oper_traits<X>::xchar>::max()
				&& val.str[0] == static_cast<oper_traits<X>::xchar>(n)
				&& 0 == wcsncmp(val.str + 1, str, n);
		}
		
		// Boolean
		constexpr explicit XOPER(bool xbool) noexcept
		{
			xltype = xltypeBool;
			val.xbool = xbool;
		}
		constexpr bool operator==(bool xbool) const
		{
			return type() == xltypeBool && !!val.xbool == xbool;
		}

		~XOPER()
		{
			deallocate();
		}
	private:
		void deallocate()
		{
			// No xlbitDLLFree or xlbitXLFree bits set.
			if (xltype == xltypeStr) {
				delete[] val.str;
			}

			xltype = xltypeNil;
		}
	};

	using OPER = XOPER<XLOPER12>;
	using OPER4 = XOPER<XLOPER>;

} // namespace xll