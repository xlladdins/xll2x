#include "../xll.h"

using namespace xll;

int oper_test()
{
	try {
		{
			OPER o(1.23);
			ensure(o.xltype == xltypeNum);
			ensure(o.val.num == 1.23);
			ensure(o == 1.23);
		}
		{
			OPER o(L"abc");
			ensure(o.xltype == xltypeStr);
			ensure(o.val.str[0] == 3);
		}
	}
	catch (const std::exception& ex) {
		MessageBoxA(0, ex.what(), "Error", MB_OK);
	}

	return 0;
}
int test_oper = oper_test();
