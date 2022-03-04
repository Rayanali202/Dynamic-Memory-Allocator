
#include "umalloc.h"
#include "csbrk.h"

//Place any variables needed here from umalloc.c or csbrk.c as an extern.
extern memory_block_t *free_head;
extern sbrk_block *sbrk_blocks;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 
 * STUDENT TODO:
 * Required to be completed for checkpoint 1:
 *      - Check that pointers in the free list point to valid free blocks. Blocks should be within the valid heap addresses: look at csbrk.h for some clues.
 *        They should also be allocated as free.
 *      - Check if any memory_blocks (free and allocated) overlap with each other. Hint: Run through the heap sequentially and check that
 *        for some memory_block n, memory_block n+1 has a sensible block_size and is within the valid heap addresses.
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Example heap check:
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1.
    /*
        memory_block_t *cur = free_head;
        while (cur) {
            if (is_allocated(cur)) {
                return -1;
            }
        }
    */

   //Checks that all free blocks are in valid memory adresses and that all free blocks
   //are allocated as free
   memory_block_t *cur = free_head;
   while(cur){
       //if free block is marked as allocated returns error flag
       if(is_allocated(cur)){
           return -1;
       }
       if(!is_memory_block(cur)){
           return -1;
       }
       sbrk_block *sbcur = sbrk_blocks;
       bool passed = false;
       uint64_t start = (uint64_t)cur;
       uint64_t end = start + (uint64_t)get_size(cur);

       //Iterates through sbrk blocks to check if free block is within a valid
       //heap address
       while(sbcur){
           if(start >= sbcur->sbrk_start && end <= sbcur->sbrk_end){
               passed = true;
               break;
           }
           sbcur = sbcur->next;
       }
       if(!passed){
           return -1;
       }
       cur = cur->next;
   }

   //Iterates through each block of memory checking that no two blocks are overlapping, 
   //extending pass the end of the arena of memory it is in, and that all blocks are 16 byte aligned
   sbrk_block *arena = sbrk_blocks;
   while(arena != NULL){
       memory_block_t *header = (memory_block_t *)arena->sbrk_start;
       uint64_t start = arena->sbrk_start;
       if(!is_memory_block(header)){
           return -1;
       }
       uint64_t end = start + (uint64_t)get_size(header);
       while(start >= arena->sbrk_start && start < arena->sbrk_end){
           if(!is_memory_block(header)){
               return -1;
           }
           if(end > arena->sbrk_end){
               return -1;
           }
           if(get_size(header) % ALIGNMENT != 0){
               return -1;
           }
           if(end == arena->sbrk_end){
               break;
           }
           header = (memory_block_t *)end;
           start = end;
           end = start + (uint64_t)get_size(header);
           
       }
       arena = arena->next;
   }
   

    return 0;
}