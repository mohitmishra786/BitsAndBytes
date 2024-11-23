section .text
global binary_search_asm

; Function prototype:
; int binary_search_asm(int* arr, int size, int target)
; rdi = array pointer
; esi = size
; edx = target value
binary_search_asm:
    xor eax, eax        ; left = 0
    lea ecx, [esi-1]    ; right = size - 1
    
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
    
    ; Use conditional moves to avoid branches
    lea r10d, [r8 + 1]  ; r10d = mid + 1
    lea r11d, [r8 - 1]  ; r11d = mid - 1
    cmovl eax, r10d     ; if arr[mid] < target, left = mid + 1
    cmovg ecx, r11d     ; if arr[mid] > target, right = mid - 1
    jmp .loop
    
.found:
    mov eax, r8d        ; return mid index
    ret
    
.not_found:
    mov eax, -1         ; return -1
    ret