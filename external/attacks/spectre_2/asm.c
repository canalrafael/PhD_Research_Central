/**
 * \brief  ARM Assembly.
 * \author Pierre AYOUB -- IRISA, CNRS
 * \date   2020
 *
 * \details Contain all ARM assembly related stuff. It can be constants or
 *          (macro-)function. The ARM ISA targeted here is only the ARMv8-A
 *          one.
 */

#include "asm.h"

int reload_t(void *ptr) {
    /* Measured times. */
    uint64_t start = 0, end = 0;
    /* Measure the time, load the byte, re-measure the time. */
    start = rdtsc();
    mem_access(ptr);
    end = rdtsc();
    mfence();
    /* Compute the elapsed time. */
    return (int)(end - start);
}

// int flush_reload_t(void *ptr) {
//     /* Measured times. */
//     uint64_t start = 0, end = 0;
//     /* Measure the time, load the byte, re-measure the time. */
//     start = rdtsc();
//     mem_access(ptr);
//     end = rdtsc();
//     mfence();
//     /* Flush the pointed byte. */
//     flush(ptr);
//     /* Compute the elapsed time. */
//     return (int)(end - start);
// }

int flush_reload_t(void *ptr) {
    uint64_t start = 0, end = 0;

    /* * 1. Flush the address from all cache levels first.
     * Use the flush macro which calls 'dc civac'.
     */
    flush(ptr);
    
    /* * 2. Ensure the flush is complete before starting the timer.
     */
    mfence_sys();
    ifence();

    /* * 3. Measure the time taken to reload the data from DRAM.
     */
    start = rdtsc();
    mem_access(ptr);
    end = rdtsc();

    /* Serialization to prevent the timer end from reordering */
    mfence_sys();

    /* Return the delta (DRAM/Cache Miss latency) */
    return (int)(end - start);
}