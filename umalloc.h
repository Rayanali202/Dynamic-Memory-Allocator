#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 16 /* The alignment of all payloads returned by umalloc */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/*
 * memory_block_t - Represents a block of memory managed by the heap. The 
 * struct can be left as is, or modified for your design.
 * In the current design bit0 is the allocated bit
 * bits 1-3 are unused.
 * and the remaining 60 bit represent the size.
 */
typedef struct memory_block_struct {
    size_t block_size_alloc;
    struct memory_block_struct *next;
} memory_block_t;

// Helper Functions, this may be editted if you change the signature in umalloc.c

/*
*  STUDENT TODO:
*      Write 1-2 sentences for each function explaining what it does. Don't just repeat the name of the function back to us.
*/

/*Checks the least significant bit in block->block_size_alloc to determine if
* memory block passed by parameter is a free or allocated block. Returns true if
* least significant bit is a 1 otherwise false if it is 0.
*/
bool is_allocated(memory_block_t *block);

/*Sets the least significant bit in block->block_size_alloc to 1. This bit is later
* used by other methods to determine if the block is an allocated or free block.
*/
void allocate(memory_block_t *block);

/*Sets the least significant bit in block->block_size_alloc to 0.
*/
void deallocate(memory_block_t *block);

/*Since the least significant bit in block->block_size_alloc is being used to
* represent whether the block of memory is allocated or not, in order to get the true
* size this method reverts the 4 least significant bits in block->block_size_alloc to 0
* and returns the true size of the block of memory.
*/
size_t get_size(memory_block_t *block);

/*Returns a pointer to the next memory block that the current block passed by the
* paremeter points to.
*/
memory_block_t *get_next(memory_block_t *block);

/*Sets the size and allocation status of memory block passed by the first parameter,
* and does so at the address in memory *block was specified to before calling the function.
*/
void put_block(memory_block_t *block, size_t size, bool alloc);

/*Returns a void pointer to the payload of a block of memory given the blocks header.
* Simply adds 1 to memory block to move up 16 bytes in memory.
*/
void *get_payload(memory_block_t *block);

/*Returns a pointer to a memory block header by casting pointer to the start of the payload
* to a memory block then subtracting one. This essentially moves the pointer back 16 bytes in memory.
*/
memory_block_t *get_block(void *payload);

/*Checks if block of memory casted to memory_block_t is really a memory block
* by checking 4th and 2nd bits to see if they are 1 
*/
bool is_memory_block(memory_block_t *block);

/* Finds and returns the first free head that is large enough to hold size amount
* of memory.
*/
memory_block_t *find(size_t size);

/*When no free block is found with enough space this block block extends the memory heap
* by adding a new sbrk block. Returns a pointer to the free block header in the memory at the
* start of the newly created sbrk block.
*/
memory_block_t *extend(size_t size);

/*Splits free block into a free and allocated block if malloc request size
* plus sizeof(memory_block_struct) is less than the size of the original free block that
* is about to be split.
*/
memory_block_t *split(memory_block_t *block, size_t size);
memory_block_t *coalesce(memory_block_t *block);


// Portion that may not be edited
int uinit();
void *umalloc(size_t size);
void ufree(void *ptr);