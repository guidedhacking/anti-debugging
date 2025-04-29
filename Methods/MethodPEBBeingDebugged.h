#pragma once

#include <windows.h>
#include <iostream>
#include "../Helpers.h"

bool MethodPEBBeingDebugged() {
    char* peb = helpers::get_peb();
    return *(peb+0x2);
}

