#include "umalloc.h"
#include "csbrk.h"
#include "ansicolors.h"
#include <stdio.h>
#include <assert.h>

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Rayan Ali ra37589" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;

/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc;
    block->block_size_alloc |= 0x4;
    block->block_size_alloc |= 0x2;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

bool is_memory_block(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & 0x4 && block->block_size_alloc & 0x2;
}

/*
 *  STUDENT TODO:
 *      Describe how you select which free block to allocate. What placement strategy are you using?
 *      I chose to implement a first fit strategy, so the first free block that has enough space to hold
 *      the memory we want to allocate is chosen.
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    //? STUDENT TODO
    memory_block_t *temp = free_head;
    while(temp != NULL){
        if(temp->block_size_alloc == size)
            return temp;
        if(temp->block_size_alloc > size + sizeof(memory_block_t))
            return split(temp, size);
        if(temp->block_size_alloc > size)
            return temp;
        temp = temp->next;
    }
    return extend(size);
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    //? STUDENT TODO
    int extendo = (int) (size / PAGESIZE);
    extendo++;
    memory_block_t *temp = (memory_block_t *)csbrk(extendo * PAGESIZE);
    put_block(temp, extendo * PAGESIZE, false);
    memory_block_t *fre = free_head;
    while(fre->next != NULL){
        fre = fre->next;
    }
    fre->next = temp;
    return find(size);
}

/*
 *  STUDENT TODO:
 *      Describe how you chose to split allocated blocks. Always? Sometimes? Never? Which end?
 *      I chose to split the blocks when there was enough space to add the size requested from malloc
 *      a memory_block_t struct.
 *      Otherwise there would be no split and the extra space would be added to the payload returned by
 *      malloc.
*/

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //? STUDENT TODO
    block->block_size_alloc = get_size(block) - size;
    deallocate(block);
    block->block_size_alloc |= 0x4;
    block->block_size_alloc |= 0x2;
    int sizzurp = (int)(get_size(block)/ALIGNMENT);
    memory_block_t *mllc = block + sizzurp;
    put_block(mllc, size, true);
    return mllc;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //? STUDENT TODO
    uint64_t end = (uint64_t)block + get_size(block);
    //if(block->next == NULL)
    //    return block;

    if(end == (uint64_t)block->next){
        block->block_size_alloc += get_size(block->next);
        block->block_size_alloc |= 0x4;
        block->block_size_alloc |= 0x2;
        block->next = block->next->next;
    }

    memory_block_t *fre = free_head;
    while(fre){
        if((uint64_t)fre + get_size(fre) == (uint64_t)block){
            fre->block_size_alloc += get_size(block);
            fre->block_size_alloc |= 0x4;
            fre->block_size_alloc |= 0x2;
            fre->next = block->next;
            break;
        }
        fre = fre->next;
    }
    return block;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    //* STUDENT TODO
    void *ptr = csbrk(3 * PAGESIZE);
    free_head = (memory_block_t *)ptr;
    put_block(free_head, 3 * PAGESIZE, false);
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    memory_block_t *mllc;
    if(size % ALIGNMENT == 0){
        mllc = find(size + sizeof(memory_block_t));
    }
    else{
        size = size + (ALIGNMENT - (size % ALIGNMENT));
        mllc = find(size + sizeof(memory_block_t));
    }
    allocate(mllc);
    if(is_allocated(free_head)){
        free_head = free_head->next;
    }
    else{
        memory_block_t *cur = free_head;
        memory_block_t *prev = NULL;
        bool updated = false;
        while(!updated && cur != NULL){
            if(is_allocated(cur)){
                prev->next = cur->next;
                updated = true;
                break;
            }
            prev = cur;
            cur = cur->next;
        }

    }
    return get_payload(mllc);
}

/*
 *  STUDENT TODO:
 *      Describe your free block insertion policy.
 *      Simply frees the memory block given in the parameter by calling
 *      deallocate(). Then traverses the free head list to find the last free block,
 *      and sets the the last free blocks next ptr to the newly deallocated block.
*/

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    //* STUDENT TODO
    memory_block_t *temp = get_block(ptr);
    memory_block_t *fre = free_head;
    deallocate(temp);
    uint64_t end = (uint64_t)temp;
    if(end < (uint64_t)free_head){
        temp->next = free_head;
        free_head = temp;
    }
    else{
        bool passed = false;
        while(fre != NULL && fre->next != NULL){
            if((uint64_t)fre < end && (uint64_t)fre->next > end){
                temp->next = fre->next;
                fre = temp;
                passed = true;
                break;
            }
            fre = fre->next;
        }
        if(!passed){
            fre->next = temp;
        }
    }
    coalesce(temp);
}