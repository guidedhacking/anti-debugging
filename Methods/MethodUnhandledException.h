#pragma once

#include <windows.h>
#include <iostream>
#include <Winternl.h>

bool hasADbgAttached = true;

LONG WINAPI GetExecutedOnUnhandledException(EXCEPTION_POINTERS * pExceptionInfo) {
    hasADbgAttached = false;

    // thx @mambda for this tip!
#ifdef _WIN64
    pExceptionInfo->ContextRecord->Rip += 1;
#else
    pExceptionInfo->ContextRecord->Eip += 1;
#endif
    return EXCEPTION_CONTINUE_EXECUTION;
}

bool MethodUnhandledException() {
    hasADbgAttached = true;

    SetUnhandledExceptionFilter(
      (LPTOP_LEVEL_EXCEPTION_FILTER) GetExecutedOnUnhandledException
    );

    //__debugbreak( );

    return hasADbgAttached;
}