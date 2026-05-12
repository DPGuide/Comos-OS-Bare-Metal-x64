#include "schneider_lang.h"

/// BARE METAL FIX: Verstecke diesen Code vor dem 32-Bit Compiler!
#ifdef __x86_64__

/// =======================================================
/// BARE METAL PAGING: PAGE TABLE ENTRY FINDEN
/// Navigiert durch PML4 -> PDPT -> PD -> PT
/// =======================================================
uint64_t* find_pte_for_address(uint64_t pml4_phys, uint64_t virtual_addr) {
    uint64_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pml4 = (uint64_t*)(pml4_phys & ~0xFFFULL);
    _15 ((pml4[pml4_idx] & 1) EQ 0) _96 0;

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & ~0xFFFULL);
    _15 ((pdpt[pdpt_idx] & 1) EQ 0) _96 0;
    
    _15 (pdpt[pdpt_idx] & 0x80) _96 &pdpt[pdpt_idx];

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFFULL);
    _15 ((pd[pd_idx] & 1) EQ 0) _96 0;
    
    _15 (pd[pd_idx] & 0x80) _96 &pd[pd_idx];

    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFFULL);
    _96 &pt[pt_idx]; 
}

/// =======================================================
/// BARE METAL FIX: NX-BIT FÜR EINEN BEREICH ENTSPERREN
/// =======================================================
extern "C" _50 disable_nx_for_app(uint64_t virtual_addr, uint64_t size_in_bytes) {
    uint64_t cr3_val;
    __asm__ _192("mov %%cr3, %0" : "=r"(cr3_val));
    
    uint64_t start_page = virtual_addr & ~0xFFFULL;
    uint64_t end_page = (virtual_addr + size_in_bytes + 0xFFF) & ~0xFFFULL;

    _39 (uint64_t page = start_page; page < end_page; page += 0x1000) {
        uint64_t* pte = find_pte_for_address(cr3_val, page);
        
        _15 (pte != 0) {
            *pte &= ~(1ULL << 63); 
            __asm__ _192("invlpg (%0)" :: "r"(page) : "memory");
        }
    }
}

#endif /// Ende des 64-Bit Blocks