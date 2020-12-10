/*说明：
*创建2个事件句柄
*静态创建事件1，包含两个事件标志，分别为event1_flag1和event1_flag2，当按下'a'键发送event1_flag1
*当按下'b'键发送event1_flag2，由于使用AND，只有按下'a'和'b'后，才会触发事件处理程序
*动态创建事件2，包含两个事件标志，分别为event2_flag1和event2_event2，当按下'c'键发送event2_flag1
*当按下'd'键发送event2_flag2，由于使用OR，任意按下'c'或者'd'后，都会触发事件处理程序
*注意：
*英文小写输入法
* Change Logs:
* Date           Author       Notes
* 2020-01-31     zhangran     the first version
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "toolkit.h"

/* 事件1句柄 */
struct tk_event event1;
/* 事件1标志 */
#define event1_flag1 (1 << 1)
#define event1_flag2 (1 << 2)

/* 事件2句柄 */
tk_event_t event2 = NULL;
/* 事件2标志 */
#define event2_flag1 (1 << 1)
#define event2_flag2 (1 << 2)

int main(int argc, char *argv[])
{
    uint32_t recved;
    /* 静态创建事件1 */
    tk_event_init(&event1);
    /* 动态创建事件2 */
    event2 = tk_event_create();

    while (1)
    {
        if (_kbhit())
        {
            /* 获取安按键 */
            int key = _getch();
            /* printf("the key you pressed is %d\n", key); */
            switch (key)
            {
            /* 按键'a'*/
            case 97:
                tk_event_send(&event1, event1_flag1);
                printf("Key 'a' pressed send event1_flag1\n");
                break;
            /* 按键'b'*/
            case 98:
                tk_event_send(&event1, event1_flag2);
                printf("Key 'b' pressed send event1_flag2\n");
                break;
            /* 按键'c'*/
            case 99:
                tk_event_send(event2, event2_flag1);
                printf("Key 'c' pressed send event2_flag1\n");
                break;
            /* 按键'd'*/
            case 100:
                tk_event_send(event2, event2_flag2);
                printf("Key 'd' pressed send event2_flag2\n");
                break;
            }
        }

        /* 事件1同时接收到了标志1和标志2，清除标志并触发事件处理程序 */
        if (true == tk_event_recv(&event1,
                                  (event1_flag1 | event1_flag2),
                                  TK_EVENT_OPTION_AND | TK_EVENT_OPTION_CLEAR,
                                  &recved))
        {
            printf("\ntrigger event and clear event\n");
            printf("event1: AND recv event 0x%x\n", recved);
        }

        /* 事件2接收到了标志1或标志2，清除标志并触发事件处理程序 */
        if (true == tk_event_recv(event2,
                                  (event2_flag1 | event2_flag2),
                                  TK_EVENT_OPTION_OR | TK_EVENT_OPTION_CLEAR,
                                  &recved))
        {
            printf("\ntrigger event and clear event\n");
            printf("event2: OR recv event 0x%x\n", recved);
        }
    }

    return 0;
}
