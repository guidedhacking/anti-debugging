#pragma once
#include <Windows.h>

namespace helpers
{
	char* get_peb()
	{
#ifndef _WIN64
		return (char*)__readfsdword(0x30);
#else
		return (char*)__readgsqword(0x60);
#endif
		return nullptr;
	}
}