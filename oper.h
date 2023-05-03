// oper.h - C++ wrapper for XLOPER12 struct
#pragma once
#include "xll.h"

namespace xll {

	struct OPER : public XLOPER12 {

		constexpr DWORD type() const
		{
			return xltype & (~xlbitDLLFree | ~xlbitXLFree);
		}

		constexpr OPER()
		{
			xltype = xltypeNil;
		}

		// 64-bit double precison floating point
		constexpr explicit OPER(double num) noexcept
		{
			xltype = xltypeNum;
			val.num = num;
		}
		OPER& operator=(double num)
		{
			free();

			return *this = OPER(num);
		}
		constexpr bool operator==(double num) const
		{
			return type() == xltypeNum && val.num == num;
		}

		// counted wide string
		OPER(size_t n, XCHAR const* str)
		{
			ensure(n <= std::numeric_limits<XCHAR>::max()/2);
			xltype = xltypeStr;
			val.str[0] = static_cast<XCHAR>(n);
			str = new XCHAR[](1 + val.str[0]);
			wcscpy_s(1 + val.str, n, str);
		}
		explicit OPER(XCHAR const* str)
			: OPER(wcslen(str), str)
		{ }
		OPER& operator=(XCHAR const* str)
		{
			free();

			return *this = OPER(str);
		}
		bool operator==(XCHAR const* str) const
		{
			size_t n = wcslen(str);

			return n <= std::numeric_limits<XCHAR>::max()
				&& val.str[0] == static_cast<XCHAR>(n)
				&& 0 == wcsncmp(val.str + 1, str, n);
		}
		
		// Boolean
		constexpr explicit OPER(bool xbool) noexcept
		{
			xltype = xltypeBool;
			val.xbool = xbool;
		}
		constexpr bool operator==(bool xbool) const
		{
			return type() == xltypeBool && !!val.xbool == xbool;
		}

		~OPER()
		{
			free();
		}
	private:
		void free()
		{
			if (xltype == xltypeStr) {
				delete[] val.str;
			}

			xltype = xltypeNil;
		}
	};

} // namespace xll