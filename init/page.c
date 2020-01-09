#include "page.h"
#include "print.h"
#include "multiboot.h"

/*
将物理页面的管理地址设定在1MB以上内核加载的结束位置之后，从这个起始位置到512MB的地址处将所有的物理内存按页划分,将每页的地址放入栈里存储。这里的数据结构栈用数组实现
512MB=512*1024=524288KB，1页大小4kb，所以栈里应该有524288kb/4096kb=128个页。根据128个页来对512mb的物理内存调度。考虑到1mb(0x00010000)以下内存有io设备，所以只操作1mb以上的内存
LD链接器已经从1mb地址作为起点，接下来只要从1mb内存地址开始，对128个页大小物理块进行操作即可
*/


//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★利用数组栈来管理页的逻辑★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
//物理内存页面管理的栈,此栈一共有128个页，以每页储存一段物理地址形式完整的划分了512mb大小的内存，栈里每个元素即数组每个元素的值是该页在物理内存的地址。
//初始化时，grub会返回数个物理段，找到满足要求的物理段(type=1等等)后，按页块大小把整个物理段划分，且每个页块在该物理段的起始地址作为元素储存在页栈中，利用页栈里该页在物理内存中的地址来控制此页块。
//初始化后，栈数组里一共有128个元素[空闲页]，每次申请页时候从最后一个页开始，返回最后一个页在物理内存中的地址，并把栈顶的指针往前移动一位[top--]，表示此时最后一页在物理内存中，不在此数组中
//最后一页现在被占用中。如果要free掉申请出去的页，只需要把该页在物理内存中的地址存入栈里即可，此时栈顶指针往后移动一位[top++]。所以在栈里的页表示空闲的，没有分配出去的，不在栈里的页都是被分配出去，被占用的
//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

static uint32_t page_stack[PAGE_MAX_SIZE+1];   //PAGE_MAX_SIZE=0x20000000/0x1000,即524288kb/4k=128个页。用int类型数组作为栈数据结构，其每个元素都是一个物理4k页的地址，最多有128页

// 物理内存管理的栈指针
static uint32_t page_stack_top;  //page_stack_top是一个地址值，指向栈顶

// 物理内存页的数量
uint32_t phy_page_count; //记录当前页的数量，即数组的元素个数

void init_page()
{
	mmap_entry_t* mmap_start_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr; //得到物理内存信息管理结构体数组的首个结构体地址,即0x00010000
	mmap_entry_t* mmap_end_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr+glb_mboot_ptr->mmap_length; //指向结构体数组的最后一个结构体地址
    mmap_entry_t* map_entry; //程序只使用1mb以上的内存，以1mb为入口使用物理内存

	for(map_entry=mmap_start_addr;map_entry<mmap_end_addr;map_entry++) //每次循环一个内存信息结构体大小
	{
		if(map_entry->type==1 && map_entry->base_addr_low==0x100000) //type为1表示可用内存,这里指定使用了从1mb开始，type为1的物理内存段
		{
		    // 把内核结束位置到结束位置的内存段，按页存储到页管理栈里
            // 这里在操作满足1mb以上，且type为1的物理内存段
			uint32_t page_addr=map_entry->base_addr_low+(uint32_t)(kern_end-kern_start);  //kern_end-kern_start表示内核已经占用的内存，从(1mb+内核已经占用内存)开始，接下里的物理内存都是可以分配的。page_addr是页的起始地址
			uint32_t length=map_entry->base_addr_low+map_entry->length_low;  //length_low指grub得到的这一段物理段的长度(grub得到的物理段有很多段，当前这段满足了1mb以上且type为1)
		    while(page_addr<length && page_addr <= PMM_MAX_SIZE)  //对当前可以使用的物理段按照页大小划分，每个页大小4k，且每个页的值是当前页块的起始物理的地址，存储在管理页的数组栈中。
			{
				page_free(page_addr);  //从第一个页开始，把该页在物理内存地址作为参数，存入页栈中保存
				page_addr=page_addr+PMM_PAGE_SIZE; //每次迭代一共页块大小
				phy_page_count++; //表示此时物理页空闲大小
			}
		}
	}
	put_str("page_initial Done\n");
}

void show_memory_map()  //因为grub获取的物理内存数据又一个[结构体类型数组储存]，此时遍历数组每个结构体元素，遍历完成后得到整个物理内存信息。根据type选项，可以看到除了ram还有其他io设备的内存地址
{
	uint32_t mmap_addr=glb_mboot_ptr->mmap_addr;  //mmap_addr是GRUB收集内存信息结构体数组的首个数组的元素
	uint32_t mmap_length=glb_mboot_ptr->mmap_length;  //length是这个结构体数组中结构体元素的个数
	put_str("Memory Map:\n");
	mmap_entry_t *mmap=(mmap_entry_t*)mmap_addr;   //mmap_entry_t类型是保存内存信息结构体的结构体(struct)类型
	                                               //之后便可以通过访问mmap来访问结构体的成员来了解物理内存的信息
     for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) //从首个结构体开始，一直到数组的最后一个结构体结束为止。对每个结构体的成员操作
	 {
	 	 put_str("base_addr=");
		 put_hex((uint32_t)mmap->base_addr_low);
		 put_str(" length=");
		 put_hex((uint32_t)mmap->length_low);
		 put_str(" type=");
		 put_hex((uint32_t)mmap->type);
		 put_str("\n");
	 }
}

uint32_t page_alloc()
{
	if(page_stack_top==0) //如果此时栈里没有空闲的页了，则无法分配
	{
		put_str("out of memeory\n");
		return;
	}
	uint32_t page=page_stack[page_stack_top--]; //页栈的栈顶指向的页当作空闲页分配出去，返回此页在物理内存中的地址
	return page;
}

void page_free(uint32_t page)
{
	if(page_stack_top==PAGE_MAX_SIZE)  //如果此时栈里全是空闲页，最多128个页，不存在free页的做法
	{
		put_str("out of page_stack\n");
		return;
	}
	page_stack[++page_stack_top]=page;  //栈指针先往前加一，再把该页在物理内存中地址放回管理页的栈中，表示此页此时不在内存中，是空闲的。
}