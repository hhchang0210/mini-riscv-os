# This Code derived from xv6-riscv (64bit)
# -- https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/swtch.S

.globl sys_timer
.align 4
sys_timer:
	/*
	這邊跟riscv-operating-system-mooc差異比較大
	這個版本的寫法非常直接和精簡。它完全沒有保存和還原通用暫存器（如 ra, sp, gp, s0-s11, t0-t6 等）。
    設計哲學：這種寫法隱含了一個重要的假設：它信任被呼叫的 C 函式 (timer_handler) 是一個非常守規矩的「葉子函式」(leaf function)。
    對 C 函式的要求：它要求 timer_handler 必須嚴格遵守 RISC-V ABI (應用程式二進位介面) 規範。這意味著 timer_handler 不可以修改任何需要由被呼叫者保存的暫存器 (callee-saved registers, 如 s0-s11)。如果它需要使用這些暫存器，它必須自己負責保存和還原。
    優點：程式碼極度簡潔，執行效率非常高，因為它省去了幾十條儲存和讀取記憶體的指令。
    缺點：非常脆弱。如果 C 語言的 timer_handler 不小心修改了一個 callee-saved 暫存器，那麼當中斷返回 (mret) 時，原本被中斷的程式將會因為暫存器被破壞而崩潰。
	*/
	# call the C timer_handler(reg_t epc, reg_t cause)
	csrr	a0, mepc
	csrr	a1, mcause
	call	timer_handler

	# timer_handler will return the return address via a0.
	csrw	mepc, a0

	mret # back to interrupt location (pc=mepc)
