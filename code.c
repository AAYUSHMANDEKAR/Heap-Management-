#include<stdio.h>
#include<stdlib.h>

typedef struct metadata_tag {
    int size;
    int free;
    struct metadata_tag* next;
} metadata;

char heap[100];

metadata* freelist = (void*)heap;

void Initialize() 
{
    freelist->size = 100 - sizeof(metadata);
    freelist->free = 1;
    freelist->next = NULL;
}

void* split(metadata* ptr, int size) 
{
    metadata* new_block = (void*)((void*)ptr + sizeof(metadata) + size);
    new_block->size = ptr->size - size - sizeof(metadata);
    new_block->free = 1;
    new_block->next = ptr->next;
    ptr->next = new_block;
    printf("New block created from %d to %d\n", (void*)new_block + sizeof(metadata), (void*)new_block + sizeof(metadata) + new_block->size);
    return new_block;
}

void* allocate(int size) 
{
    metadata* prev, * ptr;
    ptr = freelist;
    prev = NULL;
    while (ptr != NULL && (ptr->size < size  || !ptr->free)) 
    {
        prev = ptr;
        ptr = ptr->next;
    }
    if (ptr != NULL) 
    {
        if (ptr->size - size > sizeof(metadata)) 
        {
            metadata* new_block = split(ptr, size);
            ptr->size = size;
        }
        else 
        {
            printf("No block splitting\n");
        }
        ptr->free = 0;
        if (prev != NULL) 
        {
            prev->next = ptr->next;
        }
        else 
        {
            printf("Allocation at the beginning\n");
            freelist = freelist->next;
        }
        printf("Memory allocated from %d to %d\n", (void*)ptr + sizeof(metadata), (void*)ptr + sizeof(metadata) + size);
        ptr->next = NULL;
        return (void*)ptr + sizeof(metadata);
    }
    else {
        printf("Heap filled\n");
        return NULL;
    }
}

metadata* merge(metadata* a1, metadata* a2) {
    a1->next = a2->next;
    printf("Blocks of sizes %d %d are merged\n", a1->size, a2->size);
    a1->size = sizeof(metadata) + a1->size + a2->size;
    return a1;
}

void Free(void* a) 
{
    metadata* ptr, * prev, * del;
    if (a != NULL) 
    {
        del = (void*)a - sizeof(metadata);
        ptr = freelist;
        prev = NULL;
        del->free = 1;
        if(freelist == NULL)
        {
            freelist = del;
            printf("Free when freelist became NULL\n");
        }
        else if (del < freelist) {
            // Merging with the block at the beginning
            del->next = freelist;
            if ((void*)del + sizeof(metadata) + del->size == freelist) {
                del = merge(del, freelist);
            }
            freelist = del;
        } 
        else {
            while (ptr != NULL && ptr < del) 
            {
                prev = ptr;
                ptr = ptr->next;
            }
            if (prev != NULL) 
            {
                prev->next = del;
                if ((void*)prev + sizeof(metadata) + prev->size == del) {
                    del = merge(prev, del);
                }
            }
            del->next = ptr;
            if (ptr != NULL) 
            {
                // Check if merged block can further merge with the next block
                if ((void*)del + sizeof(metadata) + del->size == ptr) {
                    del = merge(del, ptr);
                }
            }
        }
    }
}


void printHeap()
{
    metadata * ptr;
    ptr = freelist;
    int i = 0;
    while(ptr != NULL)
    {
        printf("Sizes of free block number %d is -> %d\n",i,ptr->size);
        ptr = ptr->next;
        i++;
    }
}

int main() {
    Initialize();

    void* block1 = allocate(50);
    void* block2 = allocate(10);
    void* block3 = allocate(1);
    printHeap();

    Free(block2);
    printf("\n");
    printHeap();
    block2 = allocate(10);
    printf("\n");
    printHeap();
    // printf("\n");
    // printHeap();
    return 0;
}
