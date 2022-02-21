/**************************************************************************
 * C S 429 MM-lab
 * 
 * csbrk.c - A wrapper for sbrk system call. Used to keep track of calls
 * and introduce an upper limit to the amount of memory one can request at any
 * one time.
 * 
 * Copyright (c) 2021 M. Hinton. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/

#include "csbrk.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>

sbrk_block *sbrk_blocks = NULL;
size_t sbrk_bytes;

/*
 * csbrk - A wrapper for sbrk. Places a maximum on the maximum amount of memory
 * that can be requested. If tracking is enabled, keeps track of the sbrk regions
 * allocated for correctness and utilization.
 */
void *csbrk(intptr_t increment)
{
    if (increment > 16 * PAGESIZE)
    {
        printf("%ld %d %d\n", increment, 8 * PAGESIZE, increment > 8 * PAGESIZE);
        fprintf(stderr, "Memory request execeeds limit hello\n");
        return NULL;
    }

    void *ret = sbrk(increment);
#ifdef TRACK_CSBRK
    sbrk_bytes += increment;
    uint64_t sbrk_start_temp = (uint64_t)ret;
    uint64_t sbrk_end_temp = sbrk_start_temp + (uint64_t)increment;
    bool coalesced = false;
    sbrk_block *temp = sbrk_blocks;
    while (temp != NULL)
    {
        if (temp->sbrk_end == sbrk_start_temp){
            temp->sbrk_end = sbrk_end_temp;
            coalesced = true;
            break;
        }
        temp = temp->next;
    }

    if (!coalesced) {
        sbrk_block *temp = malloc(sizeof(sbrk_block));
        temp->sbrk_start = sbrk_start_temp;
        temp->sbrk_end = sbrk_end_temp;

        temp->next = sbrk_blocks;
        sbrk_blocks = temp;
    }
#endif

    return ret;
}

/*
 * check_malloc_output - Checks that a payload returned by umalloc falls within
 * one of the sbrk regions.
 */
int check_malloc_output(void *payload_start, size_t payload_length)
{
    uint64_t start_uint = (uint64_t)payload_start;
    uint64_t end_uint = start_uint + (uint64_t)payload_length;
    sbrk_block *temp = sbrk_blocks;
    while (temp != NULL)
    {
        if (start_uint >= temp->sbrk_start && end_uint <= temp->sbrk_end)
        {
            return 0;
        }
        temp = temp->next;
    }

    return -1;
}