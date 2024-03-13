#include "BeginningEngineDebugMemInfo.h"

void test(){
    int a=0;
    a++;
}
int main() {
    try {
        BeginningEngineDebugMemInfo::StackTraceManager stackTraceManager;
        BeginningEngineDebugMemInfo::print_stack_trace_for_gcc();
        void (*function)() = &test;
//        BeginningEngineDebugMemInfo::print_stack_trace_ptr(stackTraceManager.get(),
//                                                           reinterpret_cast<uintptr_t>(function));
        BeginningEngineDebugMemInfo::print_stack_trace_current(stackTraceManager.get());
    } catch (const std::runtime_error& err) {
        std::fprintf(stderr,"%s\n",err.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}