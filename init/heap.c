#include "heap.h"
#include "page.h"


#define HEAP_START 0x100000

// �����ڴ��
static int alloc_chunk(uint32_t start, uint32_t len);

// �ͷ��ڴ��
static void free_chunk(header_t *chunk);

// �з��ڴ��
static void split_chunk(header_t *chunk, uint32_t len);

// �ϲ��ڴ��
static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

// �ڴ�����ͷָ��
static header_t *heap_first;

void init_heap()
{
	heap_first = 0;
	put_str("heap_initial Done\n");
}

void *kmalloc(uint32_t len)
{
	// ����������ڴ泤�ȼ��Ϲ���ͷ�ĳ���
	// ��Ϊ���ڴ�������ͷŵ�ʱ��Ҫͨ���ýṹȥ����
	len += sizeof(header_t);   //����ĳ���=������ڴ��Ľṹ��+Ҫ�����ڴ���С

	header_t *cur_header = heap_first;   //heap_first�ڵ�һ���������Զָ��������ڴ�������е�ͷ��(�������ͷԪ��)
	header_t *prev_header = 0; 

	while (cur_header)   //���������ڴ��(����Ԫ��)��ֱ��cur_headerָ��գ�����ʱprev_headerָ��������ڴ�����������һ��Ԫ��
	                     //���;����allocated=0����δ��ʹ�ã�����length�㹻����ô��������ڴ�
	{
	    // �����ǰ�ڴ��û�б���������ҳ��ȴ��ڴ�����Ŀ�
        if (cur_header->allocated == 0 && cur_header->length >= len) 
		{
            // ���յ�ǰ�����и��ڴ�,��cur_header������ռ�õ��ڴ��
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            // ���ص�ʱ����뽫ָ��Ų������ṹ֮��
            return (void *)((uint32_t)cur_header + sizeof(header_t));  //�����и����¿�
        }
		// �������ָ��
		prev_header = cur_header;
		cur_header = cur_header->next;  
	}

	uint32_t chunk_start;

	// ��һ��ִ�иú������ʼ���ڴ����ʼλ��
	// ֮����ݵ�ǰָ���������ĳ��ȼ���
	if (prev_header)  //���˵�һ�Σ������prev_headerӦ�ö���ָ����������ڴ�����������һ��Ԫ��(�ڴ��) 
	{
		chunk_start = (uint32_t)prev_header + prev_header->length;  //(prev_header�ĵ�ַ+prev_header������ڴ���С)=�µĿ�֧����ʼ��ַ
	} 
	else   //����elseֻ���ڵ�һ��kmalloc��ִ��������
	{
		chunk_start = HEAP_START;  //��һ������ʱ����ʼ��ַ��0x100000��ʼ
		heap_first = (header_t *)chunk_start;  //�Ӵ�heap_first��Զָ��(�ѷ�����ڴ������)�е�ͷ(��)Ԫ��
	}
	// ����Ƿ���Ҫ�����ڴ�ҳ
	uint32_t page_addr=alloc_chunk(chunk_start, len);  //��len��С�ֳɶ�Ӧ������ҳ��Ȼ����page_alloc��֮����ɶ�ҳ���ӳ�䡣
	cur_header = (header_t *)chunk_start;  //chunk_start���ǸոնԿ����ҳ�������ʼ��ַ������������Ϣ��header_t�ṹ��ά��
	cur_header->prev = prev_header; //prev_header����(�ѷ�����ڴ��)�������һ��Ԫ�أ�����cur_header��������Ŀ��prevָ��ָ�������һ��Ԫ�ء�������������ڴ������ṹ
	cur_header->next = 0;  //��ʼ��nextָ��
	cur_header->allocated = 1; //�˿��ѱ�����
	cur_header->length = len; //��С��ʼ��
	cur_header->page_addr=page_addr;  //���ظÿ���׸������ҳ�ĵ�ַ,free_chunkʱ���õ�
	
	if (prev_header)  //���˵�һ��kmalloc��֮��ÿ��kmalloc����ִ����������ǰ�(�ѷ�����ڴ������)�����һ��Ԫ�ص�nextָ��ָ��ո����������ɵ��ڴ�����ṹ�塣��������˴������ά��
	{
		prev_header->next = cur_header;
	}

	return (void*)(chunk_start + sizeof(header_t));   //�Թ����ڴ����ͷ����Ϊ����ͷ������ǿ�������ʹ�õ��ڴ��
}


int alloc_chunk(uint32_t start, uint32_t len)  //�Ѹ�chunk��С��4kҳ�����֣��������һ�η����ҳ�ĵ�ַ
{
	// �����ǰ�ѵ�λ���Ѿ���������������ڴ�ҳ
	// ����ѭ�������ڴ�ҳֱ���е��㹻�Ŀ����ڴ�
	uint32_t page_addr=page_alloc();  //�ȵ��������ִ��һ��page_alloc���ѵ�һ�η����ҳ��ַ��������
	heap_max += PAGE_SIZE;
	while (heap_max < start + len)
	{
		page_alloc();  //ÿ��һ��ҳ��С�������һ��ҳ�ķ���
		heap_max += PAGE_SIZE;
	}
	return page_addr;
}

void kfree(void *p)
{
    // ָ����˵�����ṹ��������ʹ�ñ���� 0
    header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
    header->allocated = 0;  //��ʾ�˿������

    // ճ���ڴ��
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
        chunk->prev->next = 0;   //��Ϊ�˿�Ϊ���һ��ֱ��Ϊnull���ɣ���������һ���nextָ��null
    }
	uint32_t free_addr=chunk->page_addr;
    while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk)  //�ͷŵ�ǰ�ڴ��֮ǰ�������ҳ
	{
	    //���Ӧ����bug
	    page_free(free_addr);
	    heap_max -= PAGE_SIZE;
		free_addr += PAGE_SIZE;
    }
}

void split_chunk(header_t *chunk, uint32_t len)
{
    // �з��ڴ��֮ǰ�ñ�֤֮���ʣ���ڴ���������һ���ڴ�����Ĵ�С
    if (chunk->length - len > sizeof (header_t))   //���Ͻṹ��Ĵ�С�κ��㹻����
	{
        header_t *newchunk = (header_t *)((uint32_t)chunk + chunk->length);  //ԭ������ڴ�ǰ��ֳ� chunk �� newchunk������chunk������ռ�ã���Ϊnewchunk��ʼ����Ϣ
        newchunk->prev = chunk; //newchunkǰ����chunk
        newchunk->next = chunk->next;  //ԭ��chunk�ĺ���������newchunkָ��
        newchunk->allocated = 0;  //newchunk��δ��ʹ�õ�
        newchunk->length = chunk->length - len;  //�ܿ�Ĵ�С-������Ĵ�С=���п�Ĵ�С

        chunk->next = newchunk;  //�»��ֵ�chunk��nextָ��newchunk
        chunk->length = len;  //�����ִ�С��len����chunk��length
    }
}

void glue_chunk(header_t *chunk)
{
    // ������ڴ��ǰ�������ڴ����δ��ʹ����ƴ��
    if (chunk->next && chunk->next->allocated == 0)    //����¿�Ҳ�ǿ��еģ��ϲ�����
	{
        chunk->length = chunk->length + chunk->next->length;
        if (chunk->next->next)  //������¿���ڣ���ô�������¿�͵�ǰ�Ŀ��ָ����Ϣ
		{
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }

    if (chunk->prev && chunk->prev->allocated == 0)  //ͬ����
	{
        chunk->prev->length = chunk->prev->length + chunk->length;
        chunk->prev->next = chunk->next;
        if (chunk->next) 
		{
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    // ������ڴ����û�������ڴ����ֱ���ͷŵ�
    if (chunk->next == 0)   //������˿������һ��
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

