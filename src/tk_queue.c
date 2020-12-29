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
* 2019-12-12     zhangran     the first version
* 2020-01-14     zhangran     add keep fresh mode
* 2020-01-29     zhangran     add assert for developer
* 2020-02-28     zhangran     add value type define
* 2020-03-04     zhangran     add queue clean code
* 2020-06-04     zhangran     support any type
* 2020-11-28     zhangran     add queue peep&remove code
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_QUEUE

/**
 * @brief 静态初始化队列
 * 
 * @param queue 队列对象
 * @param queuepool 队列缓存区
 * @param pool_size 缓存区大小(单位字节)
 * @param queue_size 队列元素大小(单位字节)
 * @param keep_fresh 是否为保持最新模式,true：保持最新 false：默认(存满不能再存)
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool tk_queue_init(struct tk_queue *queue, void *queuepool, uint16_t pool_size,
                   uint16_t queue_size, bool keep_fresh)
{
    TK_ASSERT(queue);
    TK_ASSERT(queuepool);
    TK_ASSERT(queue_size);
    TK_ASSERT(pool_size);
    if (queue == NULL || queuepool == NULL)
        return false;
    queue->keep_fresh = keep_fresh;
    queue->queue_pool = queuepool;
    queue->queue_size = queue_size;
    queue->max_queues = pool_size / queue->queue_size;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

/**
 * @brief 静态脱离队列
 * 会使缓存区脱离与队列的关联。必须为静态方式创建的队列对象。
 * 
 * @param queue 要脱离的队列对象
 * @return true 脱离成功
 * @return false 脱离失败
 */
bool tk_queue_detach(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    queue->queue_pool = NULL;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

#ifdef TK_QUEUE_USING_CREATE
/**
 * @brief 动态创建队列
 * 
 * @param queue_size 队列元素大小(单位字节) 
 * @param max_queues 最大队列个数
 * @param keep_fresh  是否为保持最新模式,true：保持最新 false：默认(存满不能再存)
 * @return struct tk_queue* 创建的队列对象,NULL创建失败
 */
struct tk_queue *tk_queue_create(uint16_t queue_size, uint16_t max_queues,
                                 bool keep_fresh)
{
    TK_ASSERT(queue_size);
    struct tk_queue *queue;
    if ((queue = malloc(sizeof(struct tk_queue))) == NULL)
        return NULL;
    queue->queue_size = queue_size;
    queue->max_queues = max_queues;
    queue->queue_pool = malloc(queue->queue_size * queue->max_queues);
    if (queue->queue_pool == NULL)
    {
        free(queue);
        return NULL;
    }
    queue->keep_fresh = keep_fresh;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return queue;
}

/**
 * @brief 动态删除队列
 * 
 * @param queue 要删除的队列对象
 * @return true 删除成功
 * @return false 删除失败
 */
bool tk_queue_delete(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    free(queue->queue_pool);
    free(queue);
    return true;
}
#endif /* TK_QUEUE_USING_CREATE */

/**
 * @brief 清空队列
 * 
 * @param queue 要清空的队列对象
 * @return true 清除成功
 * @return false 清除失败
 */
bool tk_queue_clean(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

/**
 * @brief 查询队列当前数据长度
 * 
 * @param queue 要查询的队列对象
 * @return uint16_t 队列数据当前长度(元素个数)
 */
uint16_t tk_queue_curr_len(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    return (queue->len);
}

/**
 * @brief 判断队列是否为空
 * 
 * @param queue 要查询的队列对象
 * @return true 空
 * @return false 不为空
 */
bool tk_queue_empty(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue->len == 0)
        return true;
    return false;
}

/**
 * @brief 判断队列是否已满
 * 
 * @param queue 要查询的队列对象
 * @return true 满
 * @return false 不为满
 */
bool tk_queue_full(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue->len >= queue->max_queues)
        return true;
    return false;
}

/**
 * @brief 向队列压入(入队)1个元素数据
 * 
 * @param queue 要压入的队列对象
 * @param val 压入值
 * @return true 成功
 * @return false 失败
 */
bool tk_queue_push(struct tk_queue *queue, void *val)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_full(queue))
    {
        if (queue->keep_fresh == true)
        {
            memcpy((uint8_t *)queue->queue_pool + (queue->rear * queue->queue_size),
                   (uint8_t *)val, queue->queue_size);

            queue->rear = (queue->rear + 1) % queue->max_queues;
            queue->front = (queue->front + 1) % queue->max_queues;
            queue->len = queue->max_queues;
            return true;
        }
        return false;
    }
    else
    {
        memcpy((uint8_t *)queue->queue_pool + (queue->rear * queue->queue_size),
               (uint8_t *)val, queue->queue_size);

        queue->rear = (queue->rear + 1) % queue->max_queues;
        queue->len++;
    }
    return true;
}

/**
 * @brief 从队列弹出(出队)1个元素数据
 * 
 * @param queue 要弹出的队列对象
 * @param pval 弹出值
 * @return true 成功
 * @return false 失败
 */
bool tk_queue_pop(struct tk_queue *queue, void *pval)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_empty(queue))
    {
        return false;
    }
    else
    {
        memcpy((uint8_t *)pval,
               (uint8_t *)queue->queue_pool + (queue->front * queue->queue_size),
               queue->queue_size);

        queue->front = (queue->front + 1) % queue->max_queues;
        queue->len--;
    }
    return true;
}

/**
 * @brief 向队列压入(入队)多个元素数据
 * 
 * @param queue 要压入的队列对象
 * @param pval 压入元素首地址
 * @param len 压入元素个数
 * @return uint16_t 实际压入个数
 */
uint16_t tk_queue_push_multi(struct tk_queue *queue, void *pval, uint16_t len)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    uint8_t *u8pval = pval;
    uint16_t push_len = 0;
    while (len-- && tk_queue_push(queue, u8pval) == true)
    {
        push_len++;
        u8pval += queue->queue_size;
    }

    return push_len;
}

/**
 * @brief 从队列弹出(出队)多个元素数据
 * 
 * @param queue 要弹出的队列对象
 * @param pval 存放弹出元素的首地址
 * @param len 希望弹出的元素个数
 * @return uint16_t 实际弹出个数
 */
uint16_t tk_queue_pop_multi(struct tk_queue *queue, void *pval, uint16_t len)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    uint16_t pop_len = 0;
    uint8_t *u8pval = pval;
    if (tk_queue_empty(queue) == true)
        return false;
    while (len-- && tk_queue_pop(queue, u8pval) == true)
    {
        pop_len++;
        u8pval += queue->queue_size;
    }
    return pop_len;
}

/**
 * @brief 从队列中读取一个元素(不从队列中删除)
 * 
 * @param queue 队列对象
 * @param pval 读取值地址
 * @return true 读取成功
 * @return false 读取失败
 */
bool tk_queue_peep(struct tk_queue *queue, void *pval)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_empty(queue))
    {
        return false;
    }
    else
    {
        memcpy((uint8_t *)pval,
               (uint8_t *)queue->queue_pool + (queue->front * queue->queue_size),
               queue->queue_size);
    }
    return true;
}

/**
 * @brief 移除一个元素
 * 
 * @param queue 要移除元素的对象
 * @return true 移除成功
 * @return false 移除失败
 */
bool tk_queue_remove(struct tk_queue *queue)
{
    if (tk_queue_empty(queue))
    {
        return true;
    }
    queue->front = (queue->front + 1) % queue->max_queues;
    queue->len--;

    return true;
}
#endif /* TOOLKIT_USING_QUEUE */
