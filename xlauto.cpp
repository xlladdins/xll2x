// xlauto.cpp - xlAutoXXX functions
#include "xll.h"

using namespace xll;

#pragma region xlAutoOpen
// Called by Excel when the xll is opened.
extern "C" int __declspec(dllexport) WINAPI
xlAutoOpen(void)
{
	try {
		/*
		if (!Auto<Open>::Call()) {
			return FALSE;
		}

		for (auto& [key, arg] : AddIn::Map) {
			ensure(Register(arg));
		}

		if (!Auto<OpenAfter>::Call()) {
			return FALSE;
		}
		*/
	}
	catch (const std::exception& /*ex*/) {
		//XLL_ERROR(ex.what());

		return FALSE;
	}
	catch (...) {
		//XLL_ERROR("Unknown exception in xlAutoOpen");

		return FALSE;
	}

	return TRUE;
}
#pragma endregion xlAutoOpen


