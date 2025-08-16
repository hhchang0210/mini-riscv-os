#include "timer.h"

// a scratch area per CPU for machine-mode timer interrupts.
reg_t timer_scratch[NCPU][5];

#define interval 20000000 // cycles; about 2 second in qemu.

void timer_init()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  // int interval = 1000000; // cycles; about 1/10th second in qemu.

  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval;
  /*
  這裡是這個專案關鍵的地方, 雖然我不懂為何要在這裡準備 timervec 的資訊？在其他地方也沒有用到這個資料！
  但我猜想最重要的是讓 mscratch  可以指向一塊記憶體。(w_mscratch((reg_t)scratch))
  當程式跳到 
  trap_vector:
	# save context(registers).
	csrrw	t6, mscratch, t6	# swap t6 and mscratch
        reg_save t6
  如果沒有初始化 mscratch的話, 使用它可能程式會崩潰! (假設 mscratch = 0, t6也會是 0, 這樣reg_save t6 時從記憶體的 0 開始存, 就會當掉了！)
  
  ps： 在 riscv-operating-system-mooc 中一開始是 
  void sched_init()
{
	w_mscratch(0);
}
  設定 mscratch ＝ 0, 但之後在 entry.S
  1:
	# switch mscratch to point to the context of the next task
	csrw	mscratch, a0
  這樣就把mscratch 指向要執行的任務！
  接下來執行trap_vector:
	# save context(registers).
	csrrw	t6, mscratch, t6	# swap t6 and mscratch
	reg_save t6
  就沒有這個問題！
  
  那也就是如果 riscv-operating-system-mooc 沒有先作 switch_to 這個動作（沒有設定 mscracth），而直接等待 timer interrupt, 
  然後執行 trap_vector 應該就會發生錯誤！
  可以在 start_kerkel 裡修改一下,         
         os_main();
        while (1) {printf("here!!");};
  這樣等 timer interrupt 時就會當掉！

  */
  // prepare information in scratch[] for timervec.
  // scratch[0..2] : space for timervec to save registers.
  // scratch[3] : address of CLINT MTIMECMP register.
  // scratch[4] : desired interval (in cycles) between timer interrupts.
  reg_t *scratch = &timer_scratch[id][0];
  scratch[3] = CLINT_MTIMECMP(id);
  scratch[4] = interval;
  w_mscratch((reg_t)scratch);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

static int timer_count = 0;

void timer_handler()
{
  lib_printf("timer_handler: %d\n", ++timer_count);
  int id = r_mhartid();
  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval;
}
