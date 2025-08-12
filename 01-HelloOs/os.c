#include <stdint.h>

#define UART        0x10000000
#define UART_THR    (uint8_t*)(UART+0x00) // THR:transmitter holding register
#define UART_LSR    (uint8_t*)(UART+0x05) // LSR:line status register
#define UART_LSR_EMPTY_MASK 0x40          // LSR Bit 6: Transmitter empty; both the THR and LSR are empty

int lib_putc(char ch) {
	// 這裡跟 riscv-operating-system-mooc 略有不同
	/*
	riscv-operating-system-mooc 是檢查 bit 5。
	Bit	  Bit 5 (0x20或是 1<<%)-THRE          	Bit 6 (0x40)-TSRE
   名稱	傳送保留 (Holding) 暫存器空閒	      傳送移位 (Shift) 暫存器空閒
   意義	傳送緩衝區已空，可以接收下一個字元	    整個傳送器已空閒，傳輸已完全結束
   時機	較早變為 1 (效率更高)	              較晚變為 1 (等待時間更長)
   常見性	非常常見，是標準的實作方式	       較少見，但邏輯上依然正確
	*/
	/*
	檢查 Bit 6 (0x40)
    位元名稱：TSRE (Transmitter Shift Register Empty)
    意義：這個位元為 1 時，代表整個傳送器都處於空閒狀態，包括了 THR 和 TSR。這意味著上一個字元已經完全、徹底地從傳輸線上傳送出去了。
    工作時機：只有當上一個字元的最後一個位元都被送出後，TSRE 位元才會變為 1。
    優點：邏輯上更簡單，它等待的是一個「完全閒置」的狀態。
    缺點：等待的時間更長，CPU 的吞吐率會略低於檢查 THRE 的方式。
	*/
	while ((*UART_LSR & UART_LSR_EMPTY_MASK) == 0);
	return *UART_THR = ch; // 把 ch 寫到 THR 的位置
}

void lib_puts(char *s) {
	while (*s) lib_putc(*s++);
}

int os_main(void)
{
	lib_puts("Hello OS!\n");
	while (1) {}
	return 0;
}
