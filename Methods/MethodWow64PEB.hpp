#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C" bool check_x64_peb( );

bool MethodWow64PEB( )
{
	//auto peb32 = (char*)__readfsdword( 0x30 );
	//*( peb32 + 2 ) = 0;
	// ^ uncomment this and you'll see that the 32bit PEB check will be passed, but this one wont.

	return check_x64_peb( );
}