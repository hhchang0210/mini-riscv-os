#include "os.h"
extern void trap_vector();

void trap_init()
{
  // set the machine-mode trap handler.
  w_mtvec((reg_t)trap_vector);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
  reg_t return_pc = epc;
  /*
  0xfff：這是一個位元遮罩（bitmask），用於提取 cause 暫存器中陷阱或中斷的代碼。它的二進位表示是 0000111111111111，
  這意味著它會保留 cause 暫存器中最低的 12 位（bits 0-11）。在 RISC-V 中，這 12 位元就是用來儲存具體的陷阱或中斷代碼。
  cause & 0xfff 這個操作會將 cause 暫存器的值與 0xfff 進行位元 AND 運算，從而忽略最高位的中斷標誌和其他位元，只擷取代碼。
  */
  reg_t cause_code = cause & 0xfff;
  /*
  0x80000000：這是一個中斷（interrupt）標誌，它用於檢查 cause 暫存器中最高位（第 31 位）是否為 1。在 RISC-V 中，
  cause 暫存器的第 31 位（對於 32 位系統）或第 63 位（對於 64 位系統）用於區分同步陷阱（synchronous trap）和非同步陷阱（asynchronous trap），
  即中斷。
  */
  if (cause & 0x80000000)
  {
    /* Asynchronous trap - interrupt */
    switch (cause_code)
    {
    case 3:
      lib_puts("software interruption!\n");
      break;
    case 7:
      lib_puts("timer interruption!\n");
      // disable machine-mode timer interrupts.
      w_mie(~((~r_mie()) | (1 << 7)));
      timer_handler();
      return_pc = (reg_t)&os_kernel;
      // enable machine-mode timer interrupts.
      w_mie(r_mie() | MIE_MTIE);
      break;
    case 11:
      lib_puts("external interruption!\n");
      break;
    default:
      lib_puts("unknown async exception!\n");
      break;
    }
  }
  else
  {
    /* Synchronous trap - exception */
    lib_puts("Sync exceptions!\n");
    while (1)
    {
      /* code */
    }
  }
  return return_pc;
}
