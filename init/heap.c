#include "heap.h"
#include "page.h"


#define HEAP_START 0x100000

// 申请内存块
static int alloc_chunk(uint32_t start, uint32_t len);

// 释放内存块
static void free_chunk(header_t *chunk);

// 切分内存块
static void split_chunk(header_t *chunk, uint32_t len);

// 合并内存块
static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

// 内存块管理头指针
static header_t *heap_first;

void init_heap()
{
	heap_first = 0;
	put_str("heap_initial Done\n");
}

void *kmalloc(uint32_t len)
{
	// 所有申请的内存长度加上管理头的长度
	// 因为在内存申请和释放的时候要通过该结构去管理
	len += sizeof(header_t);   //申请的长度=管理此内存块的结构体+要申请内存块大小

	header_t *cur_header = heap_first;   //heap_first在第一次申请后，永远指向申请的内存块链表中的头块(即链表的头元素)
	header_t *prev_header = 0; 

	while (cur_header)   //遍历所有内存块(链表元素)，直到cur_header指向空，而此时prev_header指向申请的内存块链表中最后一个元素
	                     //如果途中有allocated=0，即未被使用，而且length足够，那么就用这块内存
	{
	    // 如果当前内存块没有被申请过而且长度大于待申请的块
        if (cur_header->allocated == 0 && cur_header->length >= len) 
		{
            // 按照当前长度切割内存,把cur_header当作新占用的内存块
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            // 返回的时候必须将指针挪到管理结构之后
            return (void *)((uint32_t)cur_header + sizeof(header_t));  //返回切割后的新块
        }
		// 逐次推移指针
		prev_header = cur_header;
		cur_header = cur_header->next;  
	}

	uint32_t chunk_start;

	// 第一次执行该函数则初始化内存块起始位置
	// 之后根据当前指针加上申请的长度即可
	if (prev_header)  //除了第一次，这里的prev_header应该都是指向已申请的内存块链表中最后一个元素(内存块) 
	{
		chunk_start = (uint32_t)prev_header + prev_header->length;  //(prev_header的地址+prev_header管理的内存块大小)=新的可支配起始地址
	} 
	else   //这里else只会在第一次kmalloc中执行这条流
	{
		chunk_start = HEAP_START;  //第一次申请时候起始地址从0x100000开始
		heap_first = (header_t *)chunk_start;  //从此heap_first永远指向(已分配的内存块链表)中的头(块)元素
	}
	// 检查是否需要申请内存页
	uint32_t page_addr=alloc_chunk(chunk_start, len);  //把len大小分成对应个数的页，然后再page_alloc，之后完成对页表的映射。
	cur_header = (header_t *)chunk_start;  //chunk_start就是刚刚对块完成页分配的起始地址，现在这块的信息由header_t结构体维护
	cur_header->prev = prev_header; //prev_header就是(已分配的内存块)链表最后一个元素，现在cur_header即新申请的块的prev指针指向了最后一个元素。即加入了这个内存块链表结构
	cur_header->next = 0;  //初始化next指针
	cur_header->allocated = 1; //此块已被分配
	cur_header->length = len; //大小初始化
	cur_header->page_addr=page_addr;  //返回该块的首个分配的页的地址,free_chunk时候用到
	
	if (prev_header)  //除了第一次kmalloc，之后每次kmalloc都会执行这里，作用是把(已分配的内存块链表)中最后一个元素的next指针指向刚刚新申请生成的内存块管理结构体。至此完成了此链表的维护
	{
		prev_header->next = cur_header;
	}

	return (void*)(chunk_start + sizeof(header_t));   //略过此内存管理头，因为管理头后面才是可以自由使用的内存块
}


int alloc_chunk(uint32_t start, uint32_t len)  //把该chunk大小用4k页来划分，并保存第一次分配的页的地址
{
	// 如果当前堆的位置已经到达界限则申请内存页
	// 必须循环申请内存页直到有到足够的可用内存
	uint32_t page_addr=page_alloc();  //先单独领出来执行一次page_alloc，把第一次分配的页地址保存下来
	heap_max += PAGE_SIZE;
	while (heap_max < start + len)
	{
		page_alloc();  //每隔一个页大小，就完成一次页的分配
		heap_max += PAGE_SIZE;
	}
	return page_addr;
}

void kfree(void *p)
{
    // 指针回退到管理结构，并将已使用标记置 0
    header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
    header->allocated = 0;  //表示此块空闲中

    // 粘合内存块
    glue_chunk(header);
}


void free_chunk(header_t *chunk)
{
    if (chunk->prev == 0) 
	{
        heap_first = 0;
    } 
	else 
	{
        chunk->prev->next = 0;   //因为此块为最后一块直接为null即可，所以让上一块的next指向null
    }
	uint32_t free_addr=chunk->page_addr;
    while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk)  //释放当前内存块之前所分配的页
	{
	    //这段应该有bug
	    page_free(free_addr);
	    heap_max -= PAGE_SIZE;
		free_addr += PAGE_SIZE;
    }
}

void split_chunk(header_t *chunk, uint32_t len)
{
    // 切分内存块之前得保证之后的剩余内存至少容纳一个内存管理块的大小
    if (chunk->length - len > sizeof (header_t))   //算上结构体的大小任何足够分配
	{
        header_t *newchunk = (header_t *)((uint32_t)chunk + chunk->length);  //原本大的内存前后分成 chunk 和 newchunk，现在chunk被划分占用，并为newchunk初始化信息
        newchunk->prev = chunk; //newchunk前面是chunk
        newchunk->next = chunk->next;  //原本chunk的后面现在又newchunk指向
        newchunk->allocated = 0;  //newchunk是未被使用的
        newchunk->length = chunk->length - len;  //总块的大小-被分配的大小=空闲块的大小

        chunk->next = newchunk;  //新划分的chunk的next指向newchunk
        chunk->length = len;  //被划分大小的len就是chunk的length
    }
}

void glue_chunk(header_t *chunk)
{
    // 如果该内存块前面有链内存块且未被使用则拼合
    if (chunk->next && chunk->next->allocated == 0)    //如果下块也是空闲的，合并即可
	{
        chunk->length = chunk->length + chunk->next->length;
        if (chunk->next->next)  //如果下下块存在，那么处理下下块和当前的块的指针信息
		{
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }

    if (chunk->prev && chunk->prev->allocated == 0)  //同理上
	{
        chunk->prev->length = chunk->prev->length + chunk->length;
        chunk->prev->next = chunk->next;
        if (chunk->next) 
		{
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    // 假如该内存后面没有链表内存块了直接释放掉
    if (chunk->next == 0)   //即如果此块是最后一块
	{
        free_chunk(chunk);
    }
}


void test_heap()
{
	put_str("Test kmalloc() && kfree() now ...\n");

	void *addr1 = kmalloc(50);
	put_str("kmalloc    50 byte in ");
	put_hex(addr1);
	put_str("\n");
	void *addr2 = kmalloc(500);
	put_str("kmalloc    500 byte in ");
	put_hex(addr2);
	put_str("\n");
	void *addr3 = kmalloc(5000);
	put_str("kmalloc    5000 byte in ");
	put_hex(addr3);
	put_str("\n");
	void *addr4 = kmalloc(50000);
	put_str("kmalloc    50000 byte in ");
	put_hex(addr4);
	put_str("\n");

	
	put_str("free mem in ");
	put_hex(addr4);
    kfree(addr4);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr3);
    kfree(addr3);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr2);
    kfree(addr2);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr1);
    kfree(addr1);
	put_str("\n");

}

void test_heap_again()
{
	put_str("Test kmalloc() && kfree() again ...\n");

	void *addr1 = kmalloc(50000);
	put_str("kmalloc    50000 byte in ");
	put_hex(addr1);
	put_str("\n");
	void *addr2 = kmalloc(5000);
	put_str("kmalloc    5000 byte in ");
	put_hex(addr2);
	put_str("\n");
	void *addr3 = kmalloc(500);
	put_str("kmalloc    500 byte in ");
	put_hex(addr3);
	put_str("\n");
	void *addr4 = kmalloc(50);
	put_str("kmalloc    50 byte in ");
	put_hex(addr4);
	put_str("\n");


	put_str("free mem in ");
	put_hex(addr1);
    kfree(addr1);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr2);
    kfree(addr2);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr3);
    kfree(addr3);
	put_str("\n");
	put_str("free mem in ");
	put_hex(addr4);
    kfree(addr4);
	put_str("\n");
}

