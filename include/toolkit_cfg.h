/*
* Change Logs:
* Date           Author       Notes
* 2020-01-29     zhangran     the first version
* 2020-01-31     zhangran     add event define switch
*/
#ifndef __TOOLKIT_CFG_H_
#define __TOOLKIT_CFG_H_

/* toolkit Configuration item */
#define TOOLKIT_USING_ASSERT
#define TOOLKIT_USING_QUEUE
#define TOOLKIT_USING_TIMER
#define TOOLKIT_USING_EVENT

/* toolkit queue Configuration item */
#define TK_QUEUE_USING_CREATE

/* toolkit timer Configuration item */
#define TK_TIMER_USING_CREATE
//#define TK_TIMER_USING_INTERVAL
#define TK_TIMER_USING_TIMEOUT_CALLBACK

/* toolkit event Configuration item */
#define TK_EVENT_USING_CREATE

#endif /* __TOOLKIT_CFG_H_ */
