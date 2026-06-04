#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Link external assembly clear_memory.asm
extern void clear_memory(char *target_ptr, size_t length);
//
int main(int argc, char *argv[]) {

    // minimum string length check
    if (argc < 2) {
        printf("Please provide a password string.\n");
        return 1;
    }
    // get the length of the input password string for memory allocation and clearing purposes
    size_t length = strlen(argv[1]);

    
    //  a clean, page-aligned block of memory directly from the OS kernel is created using VirtualAlloc
    char *secure_buffer = (char *)VirtualAlloc(
        NULL, 
        length + 1, 
        MEM_COMMIT | MEM_RESERVE, 
        PAGE_READWRITE // PAGE_READWRITE allows read/write access to the allocated memory
    );

    if (secure_buffer == NULL) {
        printf("VirtualAlloc failed");
        return 1;
    }

    //  copy password into our secure buffer
    strcpy(secure_buffer, argv[1]);

    //  Pin the memory page to physical RAM via VirtualLock so it doesnt slip to the pagefile.sys on disk
    printf("\n[LOCK] Securing memory page via VirtualLock()\n");
    if (VirtualLock(secure_buffer, length + 1) == 0) {
        printf("Warning: Could not lock memory page down to physical RAM");
    } else {
        printf("Success: Memory page is locked the physical hardware RAM (WS Private committed).\n");
    }

    // for  forensic verification (Process Explorer / x64dbg)
    printf("Memory Location to inspect in x64dbg: %p\n", (void *)secure_buffer);
    printf("Waiting 60 seconds....\n");
    Sleep(60000); 

  
    // clear_memory cleans the sensitive data with zero using the loop in assembly to ensure the complier doesn't remove the clearing operation
    printf("\n[PURGE] Running non-optimizable assembly clearing loop...\n");
    clear_memory(secure_buffer, length);

    // unlock the memory page, allowing the OS to manage it normally again
    VirtualUnlock(secure_buffer, length + 1);
    printf("Memory wiped and unlocked successfully. Verifying in x64dbg...\n");
    
    // Final hold to check the zeroed memory in x64dbg 
    printf("Keeping program open for 10 seconds...\n");
    Sleep(10000);

    // free the memory block back to the OS kernel as available for other applications
    VirtualFree(secure_buffer, 0, MEM_RELEASE);

    return 0;
}