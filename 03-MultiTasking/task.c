#include "task.h"
#include "lib.h"

uint8_t task_stack[MAX_TASK][STACK_SIZE];
struct context ctx_os;
struct context ctx_tasks[MAX_TASK];
struct context *ctx_now;
int taskTop=0;  // total number of task

// create a new task
int task_create(void (*task)(void))
{
	int i=taskTop++;
	ctx_tasks[i].ra = (reg_t) task;
	ctx_tasks[i].sp = (reg_t) &task_stack[i][STACK_SIZE-1];
	return i;
}

// switch to task[i]
void task_go(int i) {
	ctx_now = &ctx_tasks[i]; // ctx_now 指向要執行的函數, 假設是 task 1
	sys_switch(&ctx_os, &ctx_tasks[i]); // 把現有 os_main 的 registers 資訊都存到 ctx_os, 然後執行 ctx_tasks (假設是 task 1)
}

// switch back to os
void task_os() {
	struct context *ctx = ctx_now; // ctx 指向現在在執行的函數部份
	ctx_now = &ctx_os; // ctx_now 指向本來  os_main 的資訊, ctx_os 存有 os_main 的資訊, 沒有這一行應該也沒有關系
	sys_switch(ctx, &ctx_os); // 把現在在執行 (假設是 task 1) 的 registers 都存起來, 這樣下次才能再回到執行的狀態. 然後執行ctx_os 也就是 os_main 的資訊
	
}
