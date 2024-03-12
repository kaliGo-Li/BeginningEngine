#ifndef BEGINNING_ENGINE_DEBUG_MEM_INFO_H
#define BEGINNING_ENGINE_DEBUG_MEM_INFO_H
#ifdef _M_X64
#define IMAGE_FILE_MACHINE_TYPE IMAGE_FILE_MACHINE_AMD64
#elif defined _M_IX86
#define IMAGE_FILE_MACHINE_TYPE IMAGE_FILE_MACHINE_I386
#elif defined _M_ARM64
#define IMAGE_FILE_MACHINE_TYPE IMAGE_FILE_MACHINE_ARM64
#else
#error "Unsupported platform"
#endif
#include <string>
class  BeginningEngineDebugMemInfo {
public:
    BeginningEngineDebugMemInfo();
    ~BeginningEngineDebugMemInfo();

    BeginningEngineDebugMemInfo(const BeginningEngineDebugMemInfo&) = delete;
    BeginningEngineDebugMemInfo& operator=(const BeginningEngineDebugMemInfo&) = delete;

    bool GetFileAndLine(void* address, std::string& out_file, int& out_line);
    void AnalyzeMemoryOverflow(void* address);

private:
    void InitializeSymbolHandler();
    void CleanupSymbolHandler();
};

#endif // BEGINNING_ENGINE_DEBUG_MEM_INFO_H
