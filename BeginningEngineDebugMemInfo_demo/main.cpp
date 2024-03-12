#include <iostream>
#include <stdexcept>
#include "../BeginningEngineDebugMemInfo/BeginningEngineDebugMemInfo.h"
#pragma comment(lib, "BeginningEngineDebugMemInfo.lib")
void CauseMemoryOverflow() {
    int a = 1;
    a++;
}
int main() {
    BeginningEngineDebugMemInfo debugMemInfo;
    try {
        CauseMemoryOverflow();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    void* functionPtr = &CauseMemoryOverflow;
    try {
        debugMemInfo.AnalyzeMemoryOverflow(functionPtr);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}