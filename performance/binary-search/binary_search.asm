; binary_search.asm
section .text
global binary_search_asm

; Function prototype:
; int binary_search_asm(int* arr, int size, int target)
; rdi = array pointer
; esi = size
; edx = target value
binary_search_asm:
    push rbp
    mov rbp, rsp
    
    xor rax, rax        ; left = 0
    mov ecx, esi        ; right = size
    dec ecx             ; right = size - 1
    
.loop:
    cmp eax, ecx        ; if left > right, exit
    jg .not_found
    
    mov r8d, eax        ; r8d = left
    add r8d, ecx        ; r8d = left + right
    shr r8d, 1          ; r8d = (left + right) / 2
    
    ; Compare arr[mid] with target
    mov r9d, [rdi + r8*4]  ; r9d = arr[mid]
    cmp r9d, edx
    je .found           ; if equal, found the element
    jl .greater         ; if less, search right half
    
    ; Search left half
    lea ecx, [r8-1]     ; right = mid - 1
    jmp .loop
    
.greater:
    lea eax, [r8+1]     ; left = mid + 1
    jmp .loop
    
.found:
    mov eax, r8d        ; return mid index
    jmp .exit
    
.not_found:
    mov eax, -1         ; return -1
    
.exit:
    pop rbp
    ret