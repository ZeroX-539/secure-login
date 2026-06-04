section .text

global clear_memory

; Microsoft x64 Calling Convention:
; RCX = First parameter (integer)
; RDX = Second parameter (pointer to string buffer)
; R8  = Third parameter (integer)
; R9  = Fourth parameter (integer)
; Optional fifth and sixth parameters are passed on the stack at [RSP + 20h] and [RSP + 28h] respectively


;Microsoft x64 Calling Convention Reference:
; RCX = First parameter (char *target_ptr (securebuffer) -> Pointer to string buffer)
; RDX = Second parameter (size_t length   -> Length of string)

clear_memory:

;prologue
push rbp
mov rbp, rsp

sub rsp , 30h ; allocate 30 bytes of shadow space 

xor rax, rax  ; clear RAX register with the password with 0 

mov dword [rsp + 20h], 0 ; optional fifth parameter (integer) - passed on the stack
mov dword [rsp + 28h], 0 ; optional sixth parameter (integer) - passed on the stack


.loop:
    cmp rdx, 0            ; Check if our length counter (RDX) has hit 0
    jle .done             ; If RDX <= 0, jump to the end

    mov byte [rcx], 0     ; Hard-wipe the memory address in RCX with a null byte
    inc rcx               ; Move pointer to the next character address
    dec rdx               ; Decrement the string loop length counter
    jmp .loop             ; Loop back

.done:

add rsp, 30h ; deallocate shadow space that we allocated at the beginning of the function
mov rax, 0 ; set return value to 0 (success inducator for c )

; epilogue
mov rsp, rbp ; restore original stack pointer
pop rbp ; restore original base pointer
ret  ; return to caller


