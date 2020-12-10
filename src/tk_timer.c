/*
* Change Logs:
* Date           Author       Notes
* 2020-01-27     zhangran     the first version
* 2020-01-29     zhangran     add assert for developer
* 2020-06-04     zhangran     modify delay_tick type
* 2020-11-30     zhangran     fix bug when ticks overflow
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_TIMER
typedef uint32_t (*tk_timer_get_tick_callback)(void);
static tk_timer_get_tick_callback tk_timer_get_tick = NULL;

#ifndef TK_TIMER_USING_CREATE
static struct tk_timer tk_timer_node;
#endif /* TK_TIMER_USING_CREATE */
static struct tk_timer *tk_timer_head_node = NULL;

/**
 * @brief 查找定时器链表尾节点(内部函数)
 * 
 * @param tk_timer_start_node 开始查找的首节点
 * @return struct tk_timer* 尾节点地址
 */
static struct tk_timer *_tk_timer_find_node_tail(struct tk_timer *tk_timer_start_node)
{
    TK_ASSERT(tk_timer_start_node);
    struct tk_timer *node_curr = tk_timer_start_node;
    struct tk_timer *node_next = tk_timer_start_node;
    do
    {
        node_curr = node_next;
        node_next = node_curr->next;
    } while (node_next != NULL);
    return node_curr;
}

/**
 * @brief 插入定时器到链表(内部函数)
 * 
 * @param tk_timer_node 定时器对象
 * @return true 插入成功
 * @return false 插入失败
 */
static bool _tk_timer_insert_node_to_list(struct tk_timer *tk_timer_node)
{
    TK_ASSERT(tk_timer_node);
    struct tk_timer *node_tail = _tk_timer_find_node_tail(tk_timer_head_node);
    node_tail->next = tk_timer_node;
    tk_timer_node->prev = node_tail;
    return true;
}

/**
 * @brief 软件定时器功能初始化
 * 
 * @param get_tick_func 获取系统tick回调函数
 * @return true 初始化成功
 * @return false 初始失败
 */
bool tk_timer_func_init(uint32_t (*get_tick_func)(void))
{
    TK_ASSERT(get_tick_func);
#ifdef TK_TIMER_USING_CREATE
    struct tk_timer *tk_timer_node;
    tk_timer_node = (struct tk_timer *)malloc(sizeof(struct tk_timer));
    if (tk_timer_node == NULL)
        return false;
    tk_timer_head_node = tk_timer_node;
#else
    tk_timer_head_node = &tk_timer_node;
#endif /* TK_TIMER_USING_CREATE */
    tk_timer_head_node->prev = NULL;
    tk_timer_head_node->next = NULL;
    tk_timer_get_tick = get_tick_func;
    return true;
}

/**
 * @brief 静态初始化定时器
 * 
 * @param timer 要初始化的定时器对象
 * @param timeout_callback 定时器超时回调函数，不使用可配置为NULL
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool tk_timer_init(struct tk_timer *timer, void (*timeout_callback)(struct tk_timer *timer))
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(tk_timer_get_tick);
    TK_ASSERT(timer);
    if (tk_timer_head_node == NULL || tk_timer_get_tick == NULL)
        return false;
    timer->enable = false;
    timer->mode = TIMER_MODE_LOOP;
    timer->state = TIMER_STATE_STOP;
    timer->delay_tick = 0;
    timer->timer_tick_timeout = 0;
    timer->prev = NULL;
    timer->next = NULL;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
    timer->timeout_callback = timeout_callback;
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
    bool result = _tk_timer_insert_node_to_list(timer);
    return result;
}

/**
 * @brief 静态脱离定时器
 * 
 * @param timer 要脱离的定时器对象
 * @return true 脱离成功
 * @return false 脱离失败
 */
bool tk_timer_detach(struct tk_timer *timer)
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(timer);
    timer->prev->next = timer->next;
    if (timer->next != NULL)
        timer->next->prev = timer->prev;
    return true;
}

#ifdef TK_TIMER_USING_CREATE
/**
 * @brief 动态创建定时器
 * 
 * @param timeout_callback 定时器超时回调函数，不使用可配置为NULL
 * @return struct tk_timer* 创建的定时器对象，NULL为创建失败
 */
struct tk_timer *tk_timer_create(void (*timeout_callback)(struct tk_timer *timer))
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(tk_timer_get_tick);
    struct tk_timer *timer;
    if (tk_timer_head_node == NULL || tk_timer_get_tick == NULL)
        return NULL;
    if ((timer = malloc(sizeof(struct tk_timer))) == NULL)
        return NULL;
    timer->enable = false;
    timer->mode = TIMER_MODE_LOOP;
    timer->state = TIMER_STATE_STOP;
    timer->delay_tick = 0;
    timer->timer_tick_timeout = 0;
    timer->prev = NULL;
    timer->next = NULL;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
    timer->timeout_callback = timeout_callback;
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
    _tk_timer_insert_node_to_list(timer);
    return timer;
}

/**
 * @brief 动态删除定时器
 * 
 * @param timer 要删除的定时器对象
 * @return true 删除成功
 * @return false 删除失败
 */
bool tk_timer_delete(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    if (tk_timer_detach(timer) == true)
    {
        free(timer);
        return true;
    }
    return false;
}
#endif /* TK_TIMER_USING_CREATE */

/**
 * @brief 设置定时器启动参数(内部函数)
 * 
 * @param timer 定时器对象
 * @return true 成功
 * @return false 失败
 */
static bool _tk_timer_set_start_param(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    if (timer->delay_tick == 0 || tk_timer_get_tick == NULL)
        return false;
    timer->timer_tick_timeout = tk_timer_get_tick() + timer->delay_tick;
    timer->enable = true;
    timer->state = TIMER_STATE_RUNNING;
    return true;
}

/**
 * @brief 定时器启动
 * 
 * @param timer 要启动的定时器对象
 * @param mode 模式: 单次TIMER_MODE_SINGLE; 循环TIMER_MODE_LOOP
 * @param delay_tick 定时器时长(单位tick)
 * @return true 启动成功
 * @return false 启动失败
 */
bool tk_timer_start(struct tk_timer *timer, tk_timer_mode mode, uint32_t delay_tick)
{
    TK_ASSERT(timer);
    TK_ASSERT(delay_tick);
    timer->mode = mode;
    timer->delay_tick = delay_tick;
    bool result = _tk_timer_set_start_param(timer);
    return result;
}

/**
 * @brief 定时器停止
 * 
 * @param timer 要停止的定时器对象
 * @return true 停止成功
 * @return false 停止失败
 */
bool tk_timer_stop(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    timer->enable = false;
    timer->state = TIMER_STATE_STOP;
    return true;
}

/**
 * @brief 定时器继续
 * 
 * @param timer 要继续的定时器对象
 * @return true 继续成功
 * @return false 继续失败
 */
bool tk_timer_continue(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    timer->enable = true;
    timer->state = TIMER_STATE_RUNNING;
    return true;
}

/**
 * @brief 定时器重启
 * 
 * @param timer 要重启的定时器对象
 * @return true 重启成功
 * @return false 重启失败
 */
bool tk_timer_restart(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    bool result = _tk_timer_set_start_param(timer);
    return result;
}

/**
 * @brief 获取定时器模式
 * 
 * @param timer 要获取的定时器对象
 * @return tk_timer_mode 定时器模式：单次TIMER_MODE_SINGLE; 循环TIMER_MODE_LOOP
 */
tk_timer_mode tk_timer_get_mode(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    return timer->mode;
}

/**
 * @brief 获取定时器状态
 * 
 * @param timer 要获取的定时器对象
 * @return tk_timer_state 定时器状态 
 * TIMER_STATE_RUNNING、TIMER_STATE_STOP、TIMER_STATE_TIMEOUT
 */
tk_timer_state tk_timer_get_state(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    return timer->state;
}

/**
 * @brief 定时器处理
 * 
 * @return true 正常
 * @return false 异常
 */
bool tk_timer_loop_handler(void)
{
    struct tk_timer *timer = NULL;

    if (tk_timer_head_node != NULL)
        timer = tk_timer_head_node->next;
    else
        return false;

    while (timer != NULL)
    {
        if (timer->enable && (tk_timer_get_tick() - timer->timer_tick_timeout) < (UINT32_MAX / 2))
        {
            timer->enable = false;
            timer->state = TIMER_STATE_TIMEOUT;
#ifndef TK_TIMER_USING_INTERVAL
            if (timer->mode == TIMER_MODE_LOOP)
                tk_timer_restart(timer);
#endif /* TK_TIMER_USING_INTERVAL */
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
            if (timer->timeout_callback != NULL)
                timer->timeout_callback(timer);
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
#ifdef TK_TIMER_USING_INTERVAL
            if (timer->mode == TIMER_MODE_LOOP)
                tk_timer_restart(timer);
#endif /* TK_TIMER_USING_INTERVAL */
        }
        timer = timer->next;
    }

    return true;
}

#endif /* TOOLKIT_USING_TIMER */
