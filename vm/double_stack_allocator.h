/*
 * Copyright (c) 2019 Gil Barbosa Reis
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __DOUBLE_STACK_ALLOCATOR_H__
#define __DOUBLE_STACK_ALLOCATOR_H__

/**
 * Custom memory manager: a double stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 * The top marker is decreased, so pushed memory blocks have decreasing addresses.
 *
 */
typedef struct double_stack_allocator {
	void *buffer;  ///< Memory buffer used
	int capacity;  ///< Capacity of memory buffer
	int bottom;  ///< Bottok mark, moved when allocating from the bottom
	int top;  ///< Top mark, moved when allocating from the top
} double_stack_allocator;

/**
 * Initializes a Double Stack Allocator with a memory size.
 *
 * Upon failure, allocator will have a capacity of 0.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Size in bytes of the memory block to be allocated.
 *
 * @return Non-zero if memory was allocated successfully.
 * @return 0 otherwise.
 */
int dsa_init_with_size(double_stack_allocator *memory, unsigned int size);
/**
 * Release the memory associated with a Double Stack Allocator.
 *
 * This also zeroes out all fields in Allocator.
 *
 * It is safe to call this on a zero-initialized or previously released
 * allocator.
 *
 * @param memory The Double Stack Allocator.
 */
void dsa_release(double_stack_allocator *memory);

/**
 * Allocates a sized chunk of memory from top of Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
void *dsa_alloc_top(double_stack_allocator *memory, unsigned int size);
/**
 * Allocates a sized chunk of memory from bottom of Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
void *dsa_alloc_bottom(double_stack_allocator *memory, unsigned int size);

/**
 * Get a marker for the current top allocation state.
 *
 * The result can be used for freeing a Double Stack Allocator back to this state.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
int dsa_get_top_marker(double_stack_allocator *memory);
/**
 * Get a marker for the current bottom allocation state.
 *
 * The result can be used for freeing a Double Stack Allocator back to this state.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
int dsa_get_bottom_marker(double_stack_allocator *memory);

/**
 * Free all used memory from top of Double Stack Allocator, making it available
 * for allocation once more.
 *
 * After calling this, all top markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 */
void dsa_free_top(double_stack_allocator *memory);
/**
 * Free all used memory from bottom of Double Stack Allocator, making it
 * available for allocation once more.
 *
 * After calling this, all bottom markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 */
void dsa_free_bottom(double_stack_allocator *memory);

/**
 * Free the used memory from Double Stack Allocator top up until `marker`,
 * making it available for allocation once more.
 *
 * Memory is only freed if `marker` points to allocated memory, so invalid
 * markers are ignored.
 *
 * After calling this, markers lesser than `marker` become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 * @param marker A marker to a top allocation state.
 */
void dsa_free_top_marker(double_stack_allocator *memory, int marker);
/**
 * Free the used memory from Double Stack Allocator bottom up until `marker`,
 * making it available for allocation once more.
 *
 * Memory is only freed if `marker` points to allocated memory, so invalid
 * markers are ignored.
 *
 * After calling this, markers greater than `marker` become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 * @param marker A marker to a bottom allocation state.
 */
void dsa_free_bottom_marker(double_stack_allocator *memory, int marker);

/**
 * Retrieve a pointer to the last `size` bytes allocated from top.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are
 *         allocated from top.
 * @return NULL otherwise.
 */
void *dsa_peek_top(double_stack_allocator *memory, unsigned int size);
/**
 * Retrieve a pointer to the last `size` bytes allocated from bottom.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are
 *         allocated from bottom.
 * @return NULL otherwise.
 */
void *dsa_peek_bottom(double_stack_allocator *memory, unsigned int size);

/**
 * Get the quantity of free memory available in a Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Size in bytes of the free memory block in allocator.
 */
int dsa_available_memory(double_stack_allocator *memory);
/**
 * Get the quantity of used memory in a Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Size in bytes of the memory block used in allocator.
 */
int dsa_used_memory(double_stack_allocator *memory);

#endif

