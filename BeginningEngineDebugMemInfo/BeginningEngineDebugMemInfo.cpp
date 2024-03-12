#include "BeginningEngineDebugMemInfo.h"
#include <Windows.h>
#include <dbghelp.h>
#include <iostream>
#include <stdexcept>
#pragma comment(lib, "dbghelp.lib")

BeginningEngineDebugMemInfo::BeginningEngineDebugMemInfo() {
    InitializeSymbolHandler();
}

BeginningEngineDebugMemInfo::~BeginningEngineDebugMemInfo() {
    CleanupSymbolHandler();
}

void BeginningEngineDebugMemInfo::InitializeSymbolHandler() {
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) {
        throw std::runtime_error("Unable to initialize symbol handler, BeginningEngineDebugMemInfo failed");
    }
}

void BeginningEngineDebugMemInfo::CleanupSymbolHandler() {
    SymCleanup(GetCurrentProcess());
}

bool BeginningEngineDebugMemInfo::GetFileAndLine(void* address, std::string& out_file, int& out_line) {
    DWORD displacement = 0;
    IMAGEHLP_LINE64 line = { 0 };
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    if (SymGetLineFromAddr64(GetCurrentProcess(), reinterpret_cast<DWORD64>(address), &displacement, &line)) {
        out_file = line.FileName;
        out_line = static_cast<int>(line.LineNumber);
        return true;
    }
    return false;
}

void BeginningEngineDebugMemInfo::AnalyzeMemoryOverflow(void* address) {
    CONTEXT context = {};
    RtlCaptureContext(&context);
    STACKFRAME64 stack = {};
    memset(&stack, 0, sizeof(STACKFRAME64));
#ifdef _M_ARM64
    stack.AddrPC.Offset = reinterpret_cast<DWORD64>(address);
    stack.AddrFrame.Offset = context.Fp;
    stack.AddrStack.Offset = context.Sp;
#elif defined _M_IX86
    stack.AddrPC.Offset = reinterpret_cast<DWORD64>(address);
    stack.AddrFrame.Offset = context.Ebp;
    stack.AddrStack.Offset = context.Esp;
#elif defined _M_X64
    stack.AddrPC.Offset = reinterpret_cast<DWORD64>(address);
    stack.AddrFrame.Offset = context.Rbp;
    stack.AddrStack.Offset = context.Rsp;
#endif
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Mode = AddrModeFlat;

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DWORD imageType;
#ifdef _M_IX86
    imageType = IMAGE_FILE_MACHINE_I386;
#elif _M_X64
    imageType = IMAGE_FILE_MACHINE_AMD64;
#elif _M_ARM64
    imageType = IMAGE_FILE_MACHINE_ARM64;
#else
    imageType = IMAGE_FILE_MACHINE_UNKNOWN;
#endif

    while (StackWalk64(imageType, process, thread, &stack, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
        if (stack.AddrPC.Offset == 0) {
            break;
        }

        std::string file;
        int line;
        if (GetFileAndLine((void*)stack.AddrPC.Offset, file, line)) {
            std::cout << "at " << file << ":" << line << std::endl;
        }
    }
    SymCleanup(process);
}