/*
                  Linker File for Renesas FSP
*/

INCLUDE memory_regions.ld
INCLUDE fsp_gen.ld
_estack = __ram_thread_stack$$Limit;