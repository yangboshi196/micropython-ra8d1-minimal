#include "py/mpconfig.h"
#include "hal_data.h"

// --- 串口接收与发送的全局标志位 ---
volatile uint8_t g_rx_char = 0;
volatile bool g_rx_flag = false;
volatile bool g_tx_done = true;

// --- FSP 串口中断回调函数 ---
// 注意：这个名字必须和 e2 studio 里配置的回调函数名一模一样！
void user_uart_callback(uart_callback_args_t *p_args) {
    // 捕获到：接收到单个字符事件
    if (p_args->event == UART_EVENT_RX_CHAR) {
        g_rx_char = (uint8_t)p_args->data; // 把收到的字母存起来
        g_rx_flag = true;                  // 举起牌子告诉 Python：来信了！
    }
    // 捕获到：发送完成事件
    else if (p_args->event == UART_EVENT_TX_COMPLETE) {
        g_tx_done = true;
    }
}

// --- MicroPython 底层接口：接收单个字符 ---
int mp_hal_stdin_rx_chr(void) {
    // 死等，直到中断回调函数把标志位置为 true
    while (!g_rx_flag) {
        // 阻塞等待键盘输入
    }
    g_rx_flag = false; // 阅后即焚，放下牌子
    return g_rx_char;
}

// --- MicroPython 底层接口：发送字符串 ---
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    for (mp_uint_t i = 0; i < len; i++) {
        g_tx_done = false;
        // 呼叫底层的 FSP 发送 1 个字节
        R_SCI_B_UART_Write(&g_uart0_ctrl, (uint8_t *)&str[i], 1);
        // 死等这个字节真正从硬件引脚飞出去
        while (!g_tx_done) {
            // 阻塞等待发送完成
        }
    }
}