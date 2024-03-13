#include <cstdio>
#include <cstdlib>
#include <elfutils/libdw.h>
#include <unistd.h>
#include <libdwfl.h>
#include <execinfo.h>
#include <vector>
#include <memory>
#include "BeginningEngineDebugMemInfo.h"
namespace BeginningEngineDebugMemInfo {
    uintptr_t return_addresses[MAX_FRAMES];
    int frame_count = 0;
    void print_source_location(Dwfl_Module *mod, Dwarf_Addr addr) {
    Dwarf_Addr bias;
    Dwarf_Die *die = dwfl_module_addrdie(mod, addr, &bias);
    if (die != NULL) {
        Dwarf_Line *line;
        const char *filename;
        int lineno;
        int linecol;
        if ((line = dwarf_getsrc_die(die, addr)) != NULL) {
            // 获取该行的源文件路径
            filename = dwarf_linesrc(line, NULL, NULL);
            if (filename != NULL) {
                // 获取行号
                if (dwarf_lineno(line, &lineno) == 0) {
                    // 获取列号
                    if (dwarf_linecol(line, &linecol) == 0) {
                        printf("0x%jx: %s:%d:%d\n", (uintmax_t)addr + bias, filename, lineno, linecol);
                    } else {
                        // 如果获取列号失败，只打印行号
                        printf("0x%jx: %s:%d\n", (uintmax_t)addr + bias, filename, lineno);
                    }
                } else {
                    // 如果获取行号失败，只打印文件名
                    printf("0x%jx: %s\n", (uintmax_t)addr + bias, filename);
                }
            } else {
                // 如果获取文件名失败，只打印地址
                printf("0x%jx: ??\n", (uintmax_t)addr + bias);
            }
        } else {
            // 如果获取行信息失败，只打印地址
            printf("0x%jx: ??\n", (uintmax_t)addr + bias);
        }
    } else {
        // 如果获取die对象失败，只打印地址
        printf("0x%jx: ??\n", (uintmax_t)addr);
    }
}

void print_stack_trace_for_gcc() {
    void *trace[16];
    char **messages = (char **)NULL;
    int i, trace_size = 0;

    trace_size = backtrace(trace, 16);
    messages = backtrace_symbols(trace, trace_size);

    printf("[Stack trace]>>>\n");
    for (i = 0; i < trace_size; ++i) {
        printf("%s\n", messages[i]);
    }
    printf("<<<[End of stack trace]\n");

    free(messages);
}

void print_stack_trace_current(Dwfl *dwfl) {
    uintptr_t *current_frame, *next_frame;
    uintptr_t return_address;

#if defined(__x86_64__)
    asm volatile("mov %0, rbp" : "=r" (current_frame));
#elif defined(__i386__)
    asm volatile("mov %0, ebp" : "=r" (current_frame));
#elif defined(__aarch64__)
    asm volatile("mov %0, x29" : "=r" (current_frame));
    #else
    #error "Unsupported architecture"
#endif

    frame_count = 0;
    while (current_frame && frame_count < MAX_FRAMES) {
        next_frame = (uintptr_t *)*current_frame;
        return_address = *(current_frame + 1);
        return_addresses[frame_count++] = return_address;
        printf("Return address: 0x%lx\n", return_address);

        if (!next_frame) {
            break;
        }

        current_frame = next_frame;
    }
    for (int i = 0; i < frame_count; i++) {
        Dwfl_Module *mod = dwfl_addrmodule(dwfl, return_addresses[i]);
        if (mod) {
            Dwarf_Addr addr = 0;
            Dwarf_Die die;
            if (dwfl_module_addrdie(mod, return_addresses[i], reinterpret_cast<Dwarf_Addr *>(&die)) != NULL) {
                print_source_location(mod, return_addresses[i]);
            }
        }
    }
}
void print_stack_trace_ptr(Dwfl *dwfl, uintptr_t start_frame_address) {
    uintptr_t *current_frame = (uintptr_t *)start_frame_address;
    uintptr_t *next_frame;
    uintptr_t return_address;
    uintptr_t return_addresses[MAX_FRAMES];
    int frame_count = 0;

    while (current_frame && frame_count < MAX_FRAMES) {
        next_frame = (uintptr_t *)*current_frame;
        return_address = *(current_frame + 1);
        return_addresses[frame_count++] = return_address;
        printf("返回地址: 0x%lx\n", return_address);

        if (!next_frame) {
            break;
        }

        current_frame = next_frame;
    }

    for (int i = 0; i < frame_count; i++) {
        Dwfl_Module *mod = dwfl_addrmodule(dwfl, return_addresses[i]);
        if (mod) {
            Dwarf_Addr addr = 0;
            Dwarf_Die die;
            if (dwfl_module_addrdie(mod, return_addresses[i], &addr) != NULL) {
                print_source_location(mod, return_addresses[i]);
            }
        }
    }
}
    std::unique_ptr<char*, void(*)(void*)> make_backtrace_symbols(void** trace, int size) {
        return std::unique_ptr<char*, void(*)(void*)>(backtrace_symbols(trace, size), &std::free);
    }
}

