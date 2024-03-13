//
// Created by parallels on 24-3-13.
//

#ifndef BEGINNINGENGINEDEBUGMEMINFO_BEGINNINGENGINEDEBUGMEMINFO_H
#define BEGINNINGENGINEDEBUGMEMINFO_BEGINNINGENGINEDEBUGMEMINFO_H
#include <elfutils/libdw.h>
#include <libdwfl.h>
#include <memory>

namespace BeginningEngineDebugMemInfo {

    constexpr size_t MAX_FRAMES = 128;

    class StackTraceManager {
    public:
        StackTraceManager() {
            static const Dwfl_Callbacks callbacks = {
                    .find_elf = dwfl_linux_proc_find_elf,
                    .find_debuginfo = dwfl_standard_find_debuginfo,
                    .section_address = dwfl_offline_section_address
            };
            dwfl = dwfl_begin(&callbacks);
            if (!dwfl) {
                throw std::runtime_error("Failed to initialize libdw.");
            }

            pid_t pid = getpid();
            if (dwfl_linux_proc_report(dwfl, pid) != 0) {
                throw std::runtime_error("Failed to report on process.");
            }

            if (dwfl_report_end(dwfl, nullptr, nullptr) != 0) {
                throw std::runtime_error("Failed to report end.");
            }
        }
        ~StackTraceManager() {
            if(dwfl != nullptr) {
                dwfl_end(dwfl);
            }
        }
        Dwfl* get(){
            return dwfl;
        }
    private:
        Dwfl* dwfl = nullptr;
    };

    void print_stack_trace_for_gcc();
    void print_stack_trace_current(Dwfl *dwfl);
    void print_stack_trace_ptr(Dwfl *dwfl, uintptr_t start_frame_address);
    std::unique_ptr<char*, void(*)(void*)> make_backtrace_symbols(void** trace, int size);

}
#endif //BEGINNINGENGINEDEBUGMEMINFO_BEGINNINGENGINEDEBUGMEMINFO_H
