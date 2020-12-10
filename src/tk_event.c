/*
* Change Logs:
* Date           Author       Notes
* 2020-01-31     zhangran     the first version
* 2020-12-09     zhangran     Modify option type to prevent warning
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_EVENT

#ifdef TK_EVENT_USING_CREATE
/**
 * @brief 动态创建一个事件集
 * 
 * @return struct tk_event* 创建的事件集对象NULL为创建失败
 */
struct tk_event *tk_event_create(void)
{
    struct tk_event *event;
    if ((event = malloc(sizeof(struct tk_event))) == NULL)
        return NULL;
    event->event_set = 0;
    return event;
}

/**
 * @brief 动态删除一个事件集
 * 
 * @param event 要删除的事件集对象
 * @return true 删除成功
 * @return false 删除失败
 */
bool tk_event_delete(struct tk_event *event)
{
    TK_ASSERT(event);
    free(event);
    return true;
}
#endif /* TK_EVENT_USING_CREATE */

/**
 * @brief 静态初始化一个事件集
 * 
 * @param event 要初始化的事件集对象
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool tk_event_init(struct tk_event *event)
{
    TK_ASSERT(event);
    event->event_set = 0;
    return true;
}

/**
 * @brief 发送事件标志
 * 
 * @param event 发送目标事件集对象
 * @param event_set 事件标志，每个标志占1Bit，发送多个标志可"|""
 * @return true 发送成功
 * @return false 发送失败
 */
bool tk_event_send(struct tk_event *event, uint32_t event_set)
{
    TK_ASSERT(event);
    event->event_set |= event_set;
    return true;
}

/**
 * @brief 接收事件标志
 * 
 * @param event 接收目标事件集对象
 * @param event_set 感兴趣的标志，每个标志占1Bit，多个标志可"|"
 * @param option 操作:标志与：TK_EVENT_OPTION_AND; 标志或：TK_EVENT_OPTION_OR; 清除标志:TK_EVENT_OPTION_CLEAR
 * @param recved 事件标志
 * @return true 接收成功
 * @return false 接收失败
 */
bool tk_event_recv(struct tk_event *event, uint32_t event_set, uint8_t option, uint32_t *recved)
{
    TK_ASSERT(event);
    bool result = false;
    if (option & TK_EVENT_OPTION_AND)
    {
        if ((event->event_set & event_set) == event_set)
            result = true;
    }
    else if (option & TK_EVENT_OPTION_OR)
    {
        if (event->event_set & event_set)
            result = true;
    }
    else
    {
        TK_ASSERT(0);
    }
    if (result == true)
    {
        if (recved)
            *recved = (event->event_set & event_set);

        if (option & TK_EVENT_OPTION_CLEAR)
            event->event_set &= ~event_set;
    }
    return result;
}

#endif /* TOOLKIT_USING_EVENT */
