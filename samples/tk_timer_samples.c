/*说明：
*创建4个定时器
*定时器1和2为静态创建，并共用一个超时回调函数。
*定时器3和4为动态创建，单独有自己的超时回调函数
*到达20 tick 脱离timer2和删除timer4，只有timer1和3工作
*为了演示方便，main函数主循环1s tick加1，并循环运行tk_timer_loop_handler函数。
* Change Logs:
* Date           Author       Notes
* 2020-01-29     zhangran     the first version
*/

#include <windows.h>
#include <stdio.h>
#include "toolkit.h"

uint32_t tick = 0;
/* 定义获取系统tick回调函数 */
uint32_t get_sys_tick(void)
{
    return tick;
}

/* 定时器句柄 */
struct tk_timer timer1;
struct tk_timer timer2;
struct tk_timer *timer3 = NULL;
struct tk_timer *timer4 = NULL;

/* 定时器1和2共用的超时回调函数 */
void timer_timeout_callback(struct tk_timer *timer)
{
    if (timer == &timer1)
        printf("timeout_callback: timer1 timeout:%ld\n", get_sys_tick());
    else if (timer == &timer2)
        printf("timeout_callback: timer2 timeout:%ld\n", get_sys_tick());
}

/* 定时器3超时回调函数 */
void timer3_timeout_callback(struct tk_timer *timer)
{
    printf("timeout_callback: timer3 timeout:%ld\n", get_sys_tick());
}

/* 定时器4超时回调函数 */
void timer4_timeout_callback(struct tk_timer *timer)
{
    printf("timeout_callback: timer4 timeout:%ld\n", get_sys_tick());
}

int main(int argc, char *argv[])
{
    /* 初始化软件定时器功能，并配置tick获取回调函数*/
    tk_timer_func_init(get_sys_tick);

    /* 静态创建定时器1和2 */
    tk_timer_init(&timer1, timer_timeout_callback);
    tk_timer_init(&timer2, timer_timeout_callback);

    /* 动态创建定时器3 */
    timer3 = tk_timer_create(timer3_timeout_callback);
    if (timer3 == NULL)
    {
        printf("timer3 create error!\n");
        return 0;
    }

    /* 动态创建定时器4 */
    timer4 = tk_timer_create(timer4_timeout_callback);
    if (timer4 == NULL)
    {
        printf("timer4 create error!\n");
        return 0;
    }

    /* 启动定时器1，循环模式，5tick时长 */
    tk_timer_start(&timer1, TIMER_MODE_LOOP, 5);
    /* 启动定时器2，循环模式，3tick时长 */
    tk_timer_start(&timer2, TIMER_MODE_LOOP, 3);

    /* 启动定时器3，循环模式，5tick时长 */
    tk_timer_start(timer3, TIMER_MODE_LOOP, 5);
    /* 启动定时器3，循环模式，3tick时长 */
    tk_timer_start(timer4, TIMER_MODE_LOOP, 3);
    while (1)
    {
        /* 定时器处理程序 */
        tk_timer_loop_handler();
        printf("tick:%ld\n", get_sys_tick());

        /* 到达20 tick 脱离timer2和删除timer4 */
        if (tick == 20)
        {
            tk_timer_detach(&timer2);
            tk_timer_delete(timer4);
            printf("timer2 detach timer4 delete.\n");
        }
        tick++;
        Sleep(1000);
    }

    return 0;
}

#if 0

/* 单片机 伪代码 */

#include <stdio.h>
#include "toolkit.h"

struct tk_timer timer1;
struct tk_timer timer2;

/* stm32 滴答定时器中断,假设1ms中断一次 */
void SysTick_Handler(void)
{
	tick++;
}

/* 定义获取系统tick回调函数 */
uint32_t get_sys_tick(void)
{
	return tick;
}


/* 定时器1超时回调函数 */
void timer1_timeout_cb(struct tk_timer *timer)
{

	printf("timeout_callback: timer1 timeout.\n");
}

/* 定时器2超时回调函数 */
void timer2_timeout_cb(struct tk_timer *timer)
{
	printf("timeout_callback: timer2 timeout.\n");
}


int main(void)
{
	bsp_init();
	/* 初始化软件定时器功能，并配置tick获取回调函数*/
	tk_timer_func_init(get_sys_tick);
	/* 静态创建定时器1和2 */
	tk_timer_init(&timer1, (timeout_callback*)timer1_timeout_cb);
	tk_timer_init(&timer2, (timeout_callback*)timer2_timeout_cb);

	/* 启动定时器1，循环模式，5秒时长 */
	tk_timer_start(&timer1, TIMER_MODE_LOOP, 5000);
	/* 启动定时器2，循环模式，3秒时长 */
	tk_timer_start(&timer2, TIMER_MODE_LOOP, 1000);
	while (1)
	{
		tk_timer_loop_handler();
	}
}

#endif
