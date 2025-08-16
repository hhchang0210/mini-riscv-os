#include "os.h"

void lock_init(lock_t *lock)
{
  lock->locked = 0;
}

void lock_acquire(lock_t *lock)
{
  /*
  這裡的 spin lock 實作比riscv-operating-system-mooc 是用真正的 spin lock, riscv-operating-system-mooc 只是透過暫時關閉中斷來保證一段程式碼的執行是不可分割的
  */
  for (;;)
  {
    if (!atomic_swap(lock))
    {
      break;
    }
  }
}

void lock_free(lock_t *lock)
{
  lock->locked = 0;
}

void basic_lock()
{
  w_mstatus(r_mstatus() & ~MSTATUS_MIE);
}

void basic_unlock()
{
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}
