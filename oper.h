// oper.h - C++ wrapper for XLOPER struct
#pragma once
#include <algorithm>
#include <concepts>
#include "xll.h"

namespace xll {

	// null terminated array length
	template<class T>
	constexpr size_t len(const T* p, size_t n = 0)
	{
		return *p ? len(p + 1, n + 1) : n;
	}
#ifdef _DEBUG
	constexpr void test_len(void)
	{
		static_assert(len("") == 0);
		static_assert(len("", 1) == 1);
		static_assert(len("abc") == 3);
		static_assert(len(L"abc") == 3);
	}
#endif // _DEBUG


	template<class X>
	struct oper_traits {
	};

	template<>
	struct oper_traits<XLOPER> {
		using xtype = WORD;
		using xchar = CHAR;
		using xrw = WORD;
		using xcol = WORD;

		static const xchar xchar_max = 0xFF;
		// xrw_max
		// xcol_max
	};

	template<>
	struct oper_traits<XLOPER12> {
		using xtype = DWORD;
		using xchar = XCHAR;
		using xrw = RW;
		using xcol = COL;

		static const xchar xchar_max = 0x7FFF;
		// xrw_max
		// xcol_max
	};

	template<class X>
		//requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	inline oper_traits<X>::xtype type(const X& x)
	{
		return x.xltype & (~xlbitDLLFree | ~xlbitXLFree);
	}

	template<class X>
		//requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	inline oper_traits<X>::xrw rows(const X& x)
	{
		return x.xltype == xltypeMulti ? x.val.array.rows : 1;
	}
	template<class X>
		//requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	inline oper_traits<X>::xcol columns(const X& x)
	{
		return x.xltype == xltypeMulti ? x.val.array.columns : 1;
	}
	template<class X>
		//requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	inline auto size(const X& x)
	{
		return rows(x) * columns(x);
	}

	template<class X>
		requires std::is_same_v<X, XLOPER> || std::is_same_v<X, XLOPER12>
	struct XOPER : public X {
		using X::xltype;
		using X::val;
		using xchar = oper_traits<X>::xchar;

		// Default constructor.
		constexpr XOPER()
		{
			xltype = xltypeNil;
		}
		constexpr XOPER(const XOPER& o)
		{
			xltype = o.type(*this);
			if (xltype == xltypeStr) {
				val.str = new xchar[1 + o.val.str[0]];
				std::copy(o.val.str, o.val.str + 1 + o.val.str[0], val.str);
			}
			else if (xltype == xltypeMulti) {
				val.array.rows = o.val.array.rows;
				val.array.columns = o.val.array.columns;
				val.array.lparray = new XOPER[o.val.array.rows * o.val.array.columns];
				std::copy(o.val.array.lparray, o.val.array.lparray + o.val.array.rows * o.val.array.columns, val.array.lparray);
			}
			else {
				val = o.val;
			}
			val = o.val;
		}
		~XOPER()
		{
			deallocate();
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
			return type(*this) == xltypeNum && val.num == num;
		}

		// counted string
		XOPER(xchar const* str, size_t n)
		{
			ensure(n <= oper_traits<X>::xchar_max);

			xltype = xltypeStr;
			val.str = new xchar[1 + n];
			val.str[0] = (xchar)n;
			std::copy(str, str + n, val.str + 1);
		}
		explicit XOPER(xchar const* str)
			: XOPER(str, len(str))
		{ }
		XOPER& operator=(xchar const* str)
		{
			deallocate();

			return *this = XOPER(str);
		}
		constexpr bool operator==(xchar const* str) const
		{
			if (type(*this) != xltypeStr) {
				return false;
			}

			xchar i = 1;
			while (*str && i <= val.str[0]) {
				if (*str != val.str[i]) {
					return false;
				}
				++i;
			}

			return i == val.str[0] && *str == 0;
		}
		
		// Boolean
		constexpr explicit XOPER(bool xbool) noexcept
		{
			xltype = xltypeBool;
			val.xbool = xbool;
		}
		constexpr bool operator==(bool xbool) const
		{
			return type(*this) == xltypeBool && !!val.xbool == xbool;
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