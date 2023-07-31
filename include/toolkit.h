/*
* MIT License
* 
* Copyright (c) 2020 Cproape (911830982@qq.com)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
* Change Logs:
* Date           Author       Notes
* 2020-01-29     zhangran     the first version
* 2020-01-31     zhangran     add event extern code
* 2020-11-28     zhangran     add queue peep&remove extern code
* 2020-12-09     zhangran     Modify event option type to prevent warning
* 2023-07-31     zhangran     tk_timer adds the user_data pointer
*/
#ifndef __TOOLKIT_H_
#define __TOOLKIT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <toolkit_cfg.h>

#define TK_SW_VERSION    "1.0.7"

/* toolkit general */
#ifdef TOOLKIT_USING_ASSERT
#define TK_DEBUG(...) printf(__VA_ARGS__)
#define TK_ASSERT(EXPR)                                                   \
    if (!(EXPR))                                                          \
    {                                                                     \
        TK_DEBUG("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__); \
        while (1)                                                         \
            ;                                                             \
    }
#else
#define TK_ASSERT(EXPR) (void)EXPR
#endif /* TOOLKIT_USING_ASSERT */

/* toolkit queue */
#ifdef TOOLKIT_USING_QUEUE
struct tk_queue
{
    bool keep_fresh;
	void *queue_pool;
	uint16_t queue_size;
	uint16_t max_queues;
    uint16_t front;
    uint16_t rear;
    uint16_t len;
};
typedef struct tk_queue *tk_queue_t;

#ifdef TK_QUEUE_USING_CREATE
struct tk_queue *tk_queue_create(uint16_t queue_size, uint16_t max_queues, bool keep_fresh);
bool tk_queue_delete(struct tk_queue *queue);
#endif /* TK_QUEUE_USING_CREATE */

bool tk_queue_init(struct tk_queue *queue, void *queuepool, uint16_t pool_size, uint16_t queue_size, bool keep_fresh);
bool tk_queue_detach(struct tk_queue *queue);
bool tk_queue_clean(struct tk_queue *queue);
bool tk_queue_empty(struct tk_queue *queue);
bool tk_queue_full(struct tk_queue *queue);
bool tk_queue_peep(struct tk_queue *queue, void *pval);
bool tk_queue_remove(struct tk_queue *queue);
bool tk_queue_push(struct tk_queue *queue, void *pval);
bool tk_queue_pop(struct tk_queue *queue, void *pval);
uint16_t tk_queue_curr_len(struct tk_queue *queue);
uint16_t tk_queue_push_multi(struct tk_queue *queue, void *pval, uint16_t len);
uint16_t tk_queue_pop_multi(struct tk_queue *queue, void *pval, uint16_t len);
#endif /* TOOLKIT_USING_QUEUE */

/* toolkit timer */
#ifdef TOOLKIT_USING_TIMER

struct tk_timer;

typedef enum
{
    TIMER_STATE_RUNNING = 0,
    TIMER_STATE_STOP,
    TIMER_STATE_TIMEOUT,
} tk_timer_state;

typedef enum
{
    TIMER_MODE_SINGLE = 0,
    TIMER_MODE_LOOP,
} tk_timer_mode;

struct tk_timer
{
    bool enable;
    tk_timer_state state;
    tk_timer_mode mode;
    uint32_t delay_tick;
    uint32_t timer_tick_timeout;
    struct tk_timer *prev;
    struct tk_timer *next;
    void *user_data;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
	void(*timeout_callback)(struct tk_timer *timer);
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
};
typedef struct tk_timer *tk_timer_t;

bool tk_timer_func_init(uint32_t (*get_tick_func)(void));

#ifdef TK_TIMER_USING_CREATE
struct tk_timer *tk_timer_create(void(*timeout_callback)(struct tk_timer *timer));
bool tk_timer_delete(struct tk_timer *timer);
#endif /* TK_TIMER_USING_CREATE */
bool tk_timer_init(struct tk_timer *timer, void(*timeout_callback)(struct tk_timer *timer));
bool tk_timer_detach(struct tk_timer *timer);

bool tk_timer_start(struct tk_timer *timer, tk_timer_mode mode, uint32_t delay_tick);
bool tk_timer_stop(struct tk_timer *timer);
bool tk_timer_continue(struct tk_timer *timer);
bool tk_timer_restart(struct tk_timer *timer);
tk_timer_mode tk_timer_get_mode(struct tk_timer *timer);
tk_timer_state tk_timer_get_state(struct tk_timer *timer);
bool tk_timer_loop_handler(void);
#endif /* TOOLKIT_USING_TIMER */

/* toolkit event */
#ifdef TOOLKIT_USING_EVENT
typedef enum
{
    TK_EVENT_OPTION_AND = 0x01,
    TK_EVENT_OPTION_OR = 0x02,
    TK_EVENT_OPTION_CLEAR = 0x04,
} tk_event_option;

struct tk_event
{
    uint32_t event_set;
};
typedef struct tk_event *tk_event_t;

#ifdef TK_EVENT_USING_CREATE
struct tk_event *tk_event_create(void);
bool tk_event_delete(struct tk_event *event);
#endif /* TK_EVENT_USING_CREATE */

bool tk_event_init(struct tk_event *event);
bool tk_event_send(struct tk_event *event, uint32_t event_set);
bool tk_event_recv(struct tk_event *event, uint32_t event_set, uint8_t option, uint32_t *recved);
#endif /* TOOLKIT_USING_EVENT */

#endif /* __TOOLKIT_H_ */
