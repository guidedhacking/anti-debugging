#pragma once
#include <windows.h>
#include <iostream>
#include <mutex>

inline std::once_flag x64_trap_flag;
inline void* x64_trap_asm = nullptr;

inline bool MethodTrapFlag()
{
    __try
    {
#ifdef _WIN64
        std::call_once(x64_trap_flag, [] {
            byte function_asm[] =
            {
                0x9c,                               // pushf
                0x66, 0x81, 0x0C, 0x24, 0x00, 0x01, // or WORD PTR[rsp], 0x100
                0x9d,                               // popf
                0x90,                               // nop
                0xC3,                               // ret
            };

            x64_trap_asm = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            memcpy(x64_trap_asm, function_asm, sizeof(function_asm));

            DWORD old;
            VirtualProtect(x64_trap_asm, 0x1000, PAGE_EXECUTE_READ, &old);
        });

        reinterpret_cast<void(*)()>(x64_trap_asm)();
#else
        __asm
        {
            pushfd
            or word ptr[esp], 0x100
            popfd
            nop
        }
#endif
    }
    __except (1)
    {
        return false;
    }
    return true;
}