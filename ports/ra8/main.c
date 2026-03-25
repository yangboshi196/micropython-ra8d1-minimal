#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/pyexec.h"
#include "hal_data.h"  // <--- 它包含了所有 FSP 配置的函数和变量定义

// 对接栈顶符号，用于 GC 扫描。我们在 fsp.ld 中会将它指向真实的地址。
extern uint32_t _estack;

// 定义 MicroPython 的 GC 堆大小 (RA8D1 内存很大，这里给 128KB 演示)
static char heap[128 * 1024];

// 瑞萨 FSP 标准入口函数
void hal_entry(void) {
    // 1. 初始化引脚控制器（极其关键！激活 P408/P409 的 UART 功能）
    R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);

    // 2. 初始化并开启串口
    R_SCI_B_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);

    // 3. 裸机点火测试：向电脑发送开机问候语
    unsigned char test_msg[] = "\r\n=== RA8D1 UART IS ALIVE! ===\r\n";
    R_SCI_B_UART_Write(&g_uart0_ctrl, test_msg, sizeof(test_msg) - 1);
    
    // 粗延时，等待串口把上面的字符串发送完毕
    for (volatile int i = 0; i < 2000000; i++) {
        __asm("nop");
    }

    // 4. 初始化 GC 堆
    gc_init(heap, heap + sizeof(heap));
    
    // 5. 初始化 MicroPython 虚拟机
    mp_init();
    
    // 6. 启动 REPL (注意：这需要 uart_core.c 对接好收发函数)
    pyexec_friendly_repl();
    
    // 7. 虚拟机退出清理
    mp_deinit();
}

// 兼容 FSP 的启动流程
int main(void) {
    hal_entry();
    return 0;
}

// --- MicroPython 核心回调函数 ---

// 垃圾回收：扫描堆栈以寻找活跃对象
void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    // 从当前栈指针扫描到栈顶 (_estack)
    gc_collect_root(&dummy, ((mp_uint_t)&_estack - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

// 必须实现：处理异常跳转失败
void nlr_jump_fail(void *val) {
    (void)val;
    while (1);
}

// 必须实现：词法解析器缩进解码
void mp_lexer_decode_indent(size_t num_empty, size_t *num_indent, size_t *num_tab) {
    (void)num_empty; (void)num_indent; (void)num_tab;
}

// 必须实现：最小化的文件系统读取存根（当前不实现文件系统）
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    (void)filename;
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

// 断言失败处理
#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)file; (void)line; (void)func; (void)expr;
    while (1);
}
#endif