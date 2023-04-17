/**
 * 说明：
 *      静态方式创建queue1，缓存大小为50，模式为保持最新(当缓存已满，每存入新的数据,就会删除一个最早的数据)
 *      存入60个数据，再拿出10个数据，存入数据前后打印缓冲区。
 *
 *      动态方式创建queue2，缓存大小为50，模式为正常(存满为止)，存入 60个数据，并拿出10个数据，存入数据前
 *      后打印缓冲区。
 *
 *      静态方式创建queue3，队列元素类型为任意类型(测试为结构体)，缓存大小为10，模式为正常(存满为止)，
 *      存入 15个数据，并拿出5个数据，存入数据前后打印缓冲区。
 *
 *      若在单片机中断调用，需要注意在关键位置加入开关中断代码
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-04     zhangran     the first version
 * 2020-12-14     zhangran     optimization example notes
 * 2023-04-17     shadow3d     optimization queue print function    
 */

#include <stdio.h>
#include <string.h>
#include "toolkit.h"

/* print queue */
void printf_queue(char *str, tk_queue_t q)
{
    int i, j;

    printf("%s (%d):", str, q->len);
    for(i = 0; i < q->len; i++)
    {
        for(j = 0; j < q->queue_size; j++)
        {
            printf(" %d", *((uint8_t*)(q->queue_pool) + q->front * q->queue_size + j));
        }
        printf(",");
        q->front = (q->front + 1) % q->max_queues;
    }
    printf("\n");
}

struct test
{
	uint8_t a;
	uint8_t b;
	uint16_t c;
};

#define QUEUE1_POOL_SIZE 50
#define QUEUE2_POOL_SIZE 50
#define QUEUE3_POOL_SIZE 10

/* 队列1句柄 */
struct tk_queue queue1;
/* 队列1缓存区 */
uint8_t queue1_pool[QUEUE1_POOL_SIZE];

/* 队列3句柄 */
struct tk_queue queue3;
/* 队列3缓存区 */
struct test queue3_pool[QUEUE3_POOL_SIZE];

int main(int argc, char *argv[])
{
    int i = 0;
    uint8_t temp[100];

	/* 清空缓冲区 */
	memset(queue1_pool, 0, QUEUE1_POOL_SIZE);

    /* 静态方式创建一个循环队列"queue1",空间为queue1_pool大小,模式为保持最新 */
	tk_queue_init(&queue1, queue1_pool, sizeof(queue1_pool), sizeof(queue1_pool[0]), true);

    for (i = 0; i < sizeof(temp) / sizeof(temp[0]); i++)
        temp[i] = i;

	/* 使用前打印 queue1 */
    printf_queue("queue1_push_before", &queue1);

	/* 存入60个数据后打印 */
    tk_queue_push_multi(&queue1, temp, QUEUE1_POOL_SIZE + 10);
	printf_queue("queue1_push_after", &queue1);

	/* 拿出10个数据后打印 */
    int pop_len = tk_queue_pop_multi(&queue1, temp, 10);
    printf_queue("queue1_pop_after", &queue1);

    printf("\n");
    printf("\n");

    /* 动态方式创建一个循环队列"queue2",空间为50字节,模式为正常模式 */
    tk_queue_t queue2 = tk_queue_create(sizeof(uint8_t), QUEUE2_POOL_SIZE, false);
	memset((uint8_t *)queue2->queue_pool, 0, QUEUE2_POOL_SIZE);

    for (i = 0; i < sizeof(temp) / sizeof(temp[0]); i++)
        temp[i] = i;

	/* 使用前打 queue2 */
	printf_queue("queue2_push_before", queue2);

	/* 存入60个数据后打印 */
    tk_queue_push_multi(queue2, temp, QUEUE2_POOL_SIZE + 10);
	printf_queue("queue2_push_after", queue2);

	/* 拿出10个数据后打印 */
    pop_len = tk_queue_pop_multi(queue2, temp, 10);
    printf_queue("queue2_pop_after", queue2);

	/* 脱离队列1，腾出pool空间 */
	tk_queue_detach(&queue1);

    /* 删除队列2 */
    tk_queue_delete(queue2);

	printf("\n");
	printf("\n");

	/* 清空缓冲区 */
	memset(queue3_pool, 0, sizeof(queue3_pool));

	/* 静态方式创建一个循环队列"queue1",空间为queue1_pool大小,模式为保持最新 */
	tk_queue_init(&queue3, queue3_pool, sizeof(queue3_pool), sizeof(queue3_pool[0]), false);

	struct test test_temp[20];
	for (i = 0; i < sizeof(test_temp) / sizeof(test_temp[0]); i++)
	{
		test_temp[i].a = i;
		test_temp[i].b = i;
		test_temp[i].c = i;
	}
	/* 使用前打印 queue3 */
	printf_queue("queue3_push_before", &queue3);

	/* 存入15个数据后打印，按字节打印 */
	tk_queue_push_multi(&queue3, test_temp, 0x0F);
	printf_queue("queue3_push_after", &queue3);

	/* 拿出5个数据后打印 */
	pop_len = tk_queue_pop_multi(&queue3, test_temp, 5);
	printf_queue("queue3_pop_after", &queue3);

    getchar();
    return 0;
}
