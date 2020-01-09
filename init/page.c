#include "page.h"
#include "print.h"
#include "multiboot.h"

/*
������ҳ��Ĺ����ַ�趨��1MB�����ں˼��صĽ���λ��֮�󣬴������ʼλ�õ�512MB�ĵ�ַ�������е������ڴ水ҳ����,��ÿҳ�ĵ�ַ����ջ��洢����������ݽṹջ������ʵ��
512MB=512*1024=524288KB��1ҳ��С4kb������ջ��Ӧ����524288kb/4096kb=128��ҳ������128��ҳ����512mb�������ڴ���ȡ����ǵ�1mb(0x00010000)�����ڴ���io�豸������ֻ����1mb���ϵ��ڴ�
LD�������Ѿ���1mb��ַ��Ϊ��㣬������ֻҪ��1mb�ڴ��ַ��ʼ����128��ҳ��С�������в�������
*/


//����������������������������������������ջ������ҳ���߼���������������������������������
//�����ڴ�ҳ������ջ,��ջһ����128��ҳ����ÿҳ����һ�������ַ��ʽ�����Ļ�����512mb��С���ڴ棬ջ��ÿ��Ԫ�ؼ�����ÿ��Ԫ�ص�ֵ�Ǹ�ҳ�������ڴ�ĵ�ַ��
//��ʼ��ʱ��grub�᷵����������Σ��ҵ�����Ҫ��������(type=1�ȵ�)�󣬰�ҳ���С����������λ��֣���ÿ��ҳ���ڸ�����ε���ʼ��ַ��ΪԪ�ش�����ҳջ�У�����ҳջ���ҳ�������ڴ��еĵ�ַ�����ƴ�ҳ�顣
//��ʼ����ջ������һ����128��Ԫ��[����ҳ]��ÿ������ҳʱ������һ��ҳ��ʼ���������һ��ҳ�������ڴ��еĵ�ַ������ջ����ָ����ǰ�ƶ�һλ[top--]����ʾ��ʱ���һҳ�������ڴ��У����ڴ�������
//���һҳ���ڱ�ռ���С����Ҫfree�������ȥ��ҳ��ֻ��Ҫ�Ѹ�ҳ�������ڴ��еĵ�ַ����ջ�Ｔ�ɣ���ʱջ��ָ�������ƶ�һλ[top++]��������ջ���ҳ��ʾ���еģ�û�з����ȥ�ģ�����ջ���ҳ���Ǳ������ȥ����ռ�õ�
//���������������������������������������������������������������������������

static uint32_t page_stack[PAGE_MAX_SIZE+1];   //PAGE_MAX_SIZE=0x20000000/0x1000,��524288kb/4k=128��ҳ����int����������Ϊջ���ݽṹ����ÿ��Ԫ�ض���һ������4kҳ�ĵ�ַ�������128ҳ

// �����ڴ�����ջָ��
static uint32_t page_stack_top;  //page_stack_top��һ����ֵַ��ָ��ջ��

// �����ڴ�ҳ������
uint32_t phy_page_count; //��¼��ǰҳ���������������Ԫ�ظ���

void init_page()
{
	mmap_entry_t* mmap_start_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr; //�õ������ڴ���Ϣ����ṹ��������׸��ṹ���ַ,��0x00010000
	mmap_entry_t* mmap_end_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr+glb_mboot_ptr->mmap_length; //ָ��ṹ����������һ���ṹ���ַ
    mmap_entry_t* map_entry; //����ֻʹ��1mb���ϵ��ڴ棬��1mbΪ���ʹ�������ڴ�

	for(map_entry=mmap_start_addr;map_entry<mmap_end_addr;map_entry++) //ÿ��ѭ��һ���ڴ���Ϣ�ṹ���С
	{
		if(map_entry->type==1 && map_entry->base_addr_low==0x100000) //typeΪ1��ʾ�����ڴ�,����ָ��ʹ���˴�1mb��ʼ��typeΪ1�������ڴ��
		{
		    // ���ں˽���λ�õ�����λ�õ��ڴ�Σ���ҳ�洢��ҳ����ջ��
            // �����ڲ�������1mb���ϣ���typeΪ1�������ڴ��
			uint32_t page_addr=map_entry->base_addr_low+(uint32_t)(kern_end-kern_start);  //kern_end-kern_start��ʾ�ں��Ѿ�ռ�õ��ڴ棬��(1mb+�ں��Ѿ�ռ���ڴ�)��ʼ��������������ڴ涼�ǿ��Է���ġ�page_addr��ҳ����ʼ��ַ
			uint32_t length=map_entry->base_addr_low+map_entry->length_low;  //length_lowָgrub�õ�����һ������εĳ���(grub�õ���������кܶ�Σ���ǰ���������1mb������typeΪ1)
		    while(page_addr<length && page_addr <= PMM_MAX_SIZE)  //�Ե�ǰ����ʹ�õ�����ΰ���ҳ��С���֣�ÿ��ҳ��С4k����ÿ��ҳ��ֵ�ǵ�ǰҳ�����ʼ����ĵ�ַ���洢�ڹ���ҳ������ջ�С�
			{
				page_free(page_addr);  //�ӵ�һ��ҳ��ʼ���Ѹ�ҳ�������ڴ��ַ��Ϊ����������ҳջ�б���
				page_addr=page_addr+PMM_PAGE_SIZE; //ÿ�ε���һ��ҳ���С
				phy_page_count++; //��ʾ��ʱ����ҳ���д�С
			}
		}
	}
	put_str("page_initial Done\n");
}

void show_memory_map()  //��Ϊgrub��ȡ�������ڴ�������һ��[�ṹ���������鴢��]����ʱ��������ÿ���ṹ��Ԫ�أ�������ɺ�õ����������ڴ���Ϣ������typeѡ����Կ�������ram��������io�豸���ڴ��ַ
{
	uint32_t mmap_addr=glb_mboot_ptr->mmap_addr;  //mmap_addr��GRUB�ռ��ڴ���Ϣ�ṹ��������׸������Ԫ��
	uint32_t mmap_length=glb_mboot_ptr->mmap_length;  //length������ṹ�������нṹ��Ԫ�صĸ���
	put_str("Memory Map:\n");
	mmap_entry_t *mmap=(mmap_entry_t*)mmap_addr;   //mmap_entry_t�����Ǳ����ڴ���Ϣ�ṹ��Ľṹ��(struct)����
	                                               //֮������ͨ������mmap�����ʽṹ��ĳ�Ա���˽������ڴ����Ϣ
     for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) //���׸��ṹ�忪ʼ��һֱ����������һ���ṹ�����Ϊֹ����ÿ���ṹ��ĳ�Ա����
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
	if(page_stack_top==0) //�����ʱջ��û�п��е�ҳ�ˣ����޷�����
	{
		put_str("out of memeory\n");
		return;
	}
	uint32_t page=page_stack[page_stack_top--]; //ҳջ��ջ��ָ���ҳ��������ҳ�����ȥ�����ش�ҳ�������ڴ��еĵ�ַ
	return page;
}

void page_free(uint32_t page)
{
	if(page_stack_top==PAGE_MAX_SIZE)  //�����ʱջ��ȫ�ǿ���ҳ�����128��ҳ��������freeҳ������
	{
		put_str("out of page_stack\n");
		return;
	}
	page_stack[++page_stack_top]=page;  //ջָ������ǰ��һ���ٰѸ�ҳ�������ڴ��е�ַ�Żع���ҳ��ջ�У���ʾ��ҳ��ʱ�����ڴ��У��ǿ��еġ�
}