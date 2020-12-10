/*说明：
*静态方式创建queue1，缓存大小为50，模式为保持最新(当缓存已满，每存入新的数据,就会删除一个最早的数据)
*存入60个数据，再拿出10个数据，存入数据前后打印缓冲区。
*
*动态方式创建queue2，缓存大小为50，模式为正常(存满为止)，存入 60个数据，并拿出10个数据，存入数据前
*后打印缓冲区。
* Change Logs:
* Date           Author       Notes
* 2020-06-04     zhangran     the first version
*/

#include <stdio.h>
#include <string.h>
#include "toolkit.h"

/* 打印缓冲区 */
void printf_buffer(char *str, void *buffer, uint16_t buf_len)
{
	uint8_t *buff = buffer;
    printf("%s (%d):", str, buf_len);
    while (buf_len--)
    {
		printf("%d ", *buff);
		buff++;
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
    printf_buffer("queue1_push_before", queue1.queue_pool, QUEUE1_POOL_SIZE);
    tk_queue_push_multi(&queue1, temp, QUEUE1_POOL_SIZE + 10);
	printf_buffer("queue1_push_after", queue1.queue_pool, QUEUE1_POOL_SIZE);

    int pop_len = tk_queue_pop_multi(&queue1, temp, 10);
    printf_buffer("queue1_pop", temp, pop_len);

    printf("\n");
    printf("\n");

    /* 动态方式创建一个循环队列"queue2",空间为50字节,模式为正常模式 */
    tk_queue_t queue2 = tk_queue_create(sizeof(uint8_t), QUEUE2_POOL_SIZE, false);
	memset((uint8_t *)queue2->queue_pool, 0, QUEUE2_POOL_SIZE);

    for (i = 0; i < sizeof(temp) / sizeof(temp[0]); i++)
        temp[i] = i;
	printf_buffer("queue2_push_before", (uint8_t *)queue2->queue_pool, QUEUE2_POOL_SIZE);
    tk_queue_push_multi(queue2, temp, QUEUE2_POOL_SIZE + 10);
	printf_buffer("queue2_push_after", (uint8_t *)queue2->queue_pool, QUEUE2_POOL_SIZE);

    pop_len = tk_queue_pop_multi(queue2, temp, 10);
    printf_buffer("queue2_pop", temp, pop_len);

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
		
	printf_buffer("queue3_push_before", queue3.queue_pool, sizeof(queue3_pool));
	tk_queue_push_multi(&queue3, test_temp, 0x0F);
	printf_buffer("queue3_push_after", queue3.queue_pool, sizeof(queue3_pool));

	pop_len = tk_queue_pop_multi(&queue3, test_temp, 10);
	printf_buffer("queue3_pop", test_temp, pop_len*sizeof(queue3_pool[0]));

	

    getchar();
    return 0;
}
