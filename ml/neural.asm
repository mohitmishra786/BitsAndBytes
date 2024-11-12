section .data
    ; Input data
    input1 dq 0.0
    input2 dq 0.0
    ; Target for XOR (1,1 and 0,0 should be True, others False)
    target_true dq 1.0
    target_false dq 0.0
    ; Weights and Bias
    weight1 dq 0.5
    weight2 dq 0.5
    bias dq -0.5
    ; Learning rate
    learning_rate dq 0.1
    ; Training settings
    num_epochs equ 1000
    newline db 0x0a ; Newline character
    epoch_msg db 'Epoch: ', 0
    true_msg db 'True', 0x0a
    false_msg db 'False', 0x0a
    usage_msg db 'Usage: ./neural <input1> <input2>', 0x0a, 0
    usage_len equ $ - usage_msg

section .bss
    ; Output of the neuron
    output resq 1
    error resq 1
    ; Epoch number
    epoch_num resb 4 ; 3-digit epoch number plus null terminator

section .text
global _start

_start:
    ; Check if the user provided two command-line arguments
    mov rdi,[rsp] ; Number of arguments
    cmp rdi, 3
    jne .usage_error

    ; Parse first argument
    mov rsi,[rsp + 16] ; First argument string
    call atoi
    movsd[input1], xmm0

    ; Parse second argument
    mov rsi,[rsp + 24] ; Second argument string
    call atoi
    movsd[input2], xmm0

    ; Training loop
    mov r12d, num_epochs ; Initialize loop counter

.train_loop:
    ; Load inputs into FPU
    finit ; Initialize FPU
    movsd xmm0,[input1]
    movsd xmm1,[input2]

    ; Determine target based on inputs
    ucomisd xmm0, xmm1 ; Compare input1 and input2
    jne .set_false_target
        movsd xmm2,[target_true]
        jmp .continue_training

.set_false_target:
    movsd xmm2,[target_false]

.continue_training:
    ; Multiply by weights
    mulsd xmm0,[weight1]
    mulsd xmm1,[weight2]

    ; Add bias
    addsd xmm0, xmm1
    addsd xmm0,[bias]

    ; Apply step function (simplified activation)
    xorpd xmm1, xmm1 ; Load 0.0 into xmm1
    ucomisd xmm0, xmm1 ; Compare xmm0 with xmm1
    jb .zero ; If below 0, set output to 0
        movsd[output], xmm2 ; Set output based on target
        jmp .update_weights
    
.zero:
    movsd[output], xmm1 ; Set output to 0

.update_weights:
    ; Update weights and bias
    movsd xmm3, xmm2 ; Store target
    subsd xmm3,[output] ; Error = Target - Output
    movsd[error], xmm3

    mulsd xmm3,[learning_rate]
    mulsd xmm3,[input1]   ; Weight1 change = Error * Learning Rate * Input1
    addsd xmm3,[weight1]  ; New weight1 = Old weight1 + Weight1 change
    movsd[weight1], xmm3

    mulsd xmm3,[learning_rate]
    mulsd xmm3,[input2]   ; Weight2 change = Error * Learning Rate * Input2
    addsd xmm3,[weight2]  ; New weight2 = Old weight2 + Weight2 change
    movsd[weight2], xmm3

    mulsd xmm3,[learning_rate]
    addsd xmm3,[bias]    ; New bias = Old bias + Error * Learning Rate
    movsd[bias], xmm3

    ; Print the current epoch number every 100 epochs
    mov ecx, r12d
    and ecx, 0x63 ; Check if divisible by 100 (since 100 in hex is 0x64)
    jnz .skip_print_epoch
        push r12
        mov rax, 1 ; sys_write
        mov rdi, 1 ; stdout
        mov rsi, epoch_msg
        mov rdx, 7 ; Length of the epoch message
        syscall

        ; Convert the loop counter to a 3-digit string
        mov rsi, epoch_num
        mov rdi, r12
        mov rcx, 100
        xor rdx, rdx
        div rcx
        add al, '0'
        mov[rsi], al
        mov eax, edx
        mov rcx, 10
        xor rdx, rdx
        div rcx
        add al, '0'
        add ah, '0'
        mov[rsi + 1], ax
        mov byte[rsi + 3], 0 ; Null terminator

        mov rax, 1 ; sys_write
        mov rdi, 1 ; stdout
        mov rsi, epoch_num
        mov rdx, 3 ; Length of the epoch number (3 digits)
        syscall

        ; Print a newline
        mov rax, 1 ; sys_write
        mov rdi, 1 ; stdout
        mov rsi, newline
        mov rdx, 1 ; 1 byte (size of newline character)
        syscall

        pop r12

.skip_print_epoch:
    ; Loop back if we haven't reached the desired number of epochs
    dec r12d
    jnz .train_loop

    ; Compute and print the final output
    finit
    movsd xmm0,[input1]
    movsd xmm1,[input2]
    mulsd xmm0,[weight1]
    mulsd xmm1,[weight2]
    addsd xmm0, xmm1
    addsd xmm0,[bias]
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    jb .false
        mov rax, 1 ; sys_write
        mov rdi, 1 ; stdout
        mov rsi, true_msg
        mov rdx, 5 ; Length of the true message
        syscall
        jmp .exit
    
.false:
    mov rax, 1 ; sys_write
    mov rdi, 1 ; stdout
    mov rsi, false_msg
    mov rdx, 6 ; Length of the false message
    syscall

.exit:
    mov rax, 60 ; sys_exit
    xor edi, edi
    syscall

.usage_error:
    mov rax, 1 ; sys_write
    mov rdi, 1 ; stdout
    mov rsi, usage_msg
    mov rdx, usage_len
    syscall
    mov rax, 60 ; sys_exit
    mov edi, 1   ; Exit with error code
    syscall

atoi:
    xor rax, rax
.loop:
    movzx rcx, byte[rsi]
    test cl, cl
    jz .done
    cmp cl, '0'
    jb .done
    cmp cl, '9'
    ja .done
    sub cl, '0'
    imul rax, 10
    add rax, rcx
    inc rsi
    jmp .loop
.done:
    cvtsi2sd xmm0, rax
    ret