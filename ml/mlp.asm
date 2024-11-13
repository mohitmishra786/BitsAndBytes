; Multi-Layer Perceptron Implementation in x86_64 Assembly
; For Linux x86_64

section .data
    ; Format strings
    fmt_epoch db "Epoch %d, Error: %f", 10, 0
    fmt_final db "Final Output: %f", 10, 0
    fmt_usage db "Usage: %s <input1> <input2>", 10, 0
    
    ; Network parameters
    learning_rate dq 0.1
    one dq 1.0
    zero dq 0.0
    
    ; Network architecture
    input_size equ 2
    hidden_size equ 4
    output_size equ 1
    num_epochs equ 1000

    ; Network weights and biases (initialized with small random values)
    hidden_w1 dq 0.15
    hidden_w2 dq 0.25
    hidden_w3 dq 0.20
    hidden_w4 dq 0.30
    hidden_w5 dq 0.25
    hidden_w6 dq 0.35
    hidden_w7 dq 0.30
    hidden_w8 dq 0.40
    
    hidden_b1 dq 0.1
    hidden_b2 dq 0.1
    hidden_b3 dq 0.1
    hidden_b4 dq 0.1
    
    output_w1 dq 0.40
    output_w2 dq 0.45
    output_w3 dq 0.50
    output_w4 dq 0.55
    
    output_bias dq 0.1

section .bss
    ; Input values
    input1 resq 1
    input2 resq 1
    
    ; Hidden layer outputs
    hidden_out1 resq 1
    hidden_out2 resq 1
    hidden_out3 resq 1
    hidden_out4 resq 1
    
    hidden_delta1 resq 1
    hidden_delta2 resq 1
    hidden_delta3 resq 1
    hidden_delta4 resq 1
    
    ; Output layer values
    output resq 1
    output_delta resq 1
    target resq 1
    error resq 1

section .text
    global main
    extern printf
    extern atof
    extern exit

main:
    push rbp
    mov rbp, rsp
    
    ; Check argument count
    cmp rdi, 3
    jne usage_error
    
    ; Save args
    mov r12, rsi        ; Save argv
    
    ; Convert first input
    mov rdi, [r12 + 8]  ; argv[1]
    call atof
    movsd [input1], xmm0
    
    ; Convert second input
    mov rdi, [r12 + 16] ; argv[2]
    call atof
    movsd [input2], xmm0
    
    ; Set target based on inputs (XOR function)
    movsd xmm0, [input1]
    movsd xmm1, [input2]
    xorpd xmm2, xmm2
    ucomisd xmm0, xmm1
    setnz al
    movzx rax, al
    cvtsi2sd xmm2, rax
    movsd [target], xmm2
    
    ; Training loop
    mov r13d, num_epochs    ; Epoch counter
    
train_loop:
    ; Forward pass
    call forward_pass
    
    ; Backward pass
    call backward_pass
    
    ; Update weights
    call update_weights
    
    ; Print progress every 100 epochs
    mov eax, r13d
    mov ecx, 100
    cdq
    idiv ecx
    test edx, edx
    jnz skip_print
    
    ; Print epoch and error
    mov rdi, fmt_epoch
    mov esi, r13d          ; Current epoch
    movsd xmm0, [error]    ; Current error
    mov eax, 1             ; One float argument
    call printf
    
skip_print:
    dec r13d
    jnz train_loop
    
    ; Print final output
    mov rdi, fmt_final
    movsd xmm0, [output]
    mov eax, 1
    call printf
    
    ; Exit program
    xor edi, edi
    call exit

forward_pass:
    push rbp
    mov rbp, rsp
    
    ; Calculate hidden layer outputs
    ; Hidden neuron 1
    movsd xmm0, [input1]
    mulsd xmm0, [hidden_w1]
    movsd xmm1, [input2]
    mulsd xmm1, [hidden_w2]
    addsd xmm0, xmm1
    addsd xmm0, [hidden_b1]
    call relu
    movsd [hidden_out1], xmm0
    
    ; Hidden neuron 2
    movsd xmm0, [input1]
    mulsd xmm0, [hidden_w3]
    movsd xmm1, [input2]
    mulsd xmm1, [hidden_w4]
    addsd xmm0, xmm1
    addsd xmm0, [hidden_b2]
    call relu
    movsd [hidden_out2], xmm0
    
    ; Hidden neuron 3
    movsd xmm0, [input1]
    mulsd xmm0, [hidden_w5]
    movsd xmm1, [input2]
    mulsd xmm1, [hidden_w6]
    addsd xmm0, xmm1
    addsd xmm0, [hidden_b3]
    call relu
    movsd [hidden_out3], xmm0
    
    ; Hidden neuron 4
    movsd xmm0, [input1]
    mulsd xmm0, [hidden_w7]
    movsd xmm1, [input2]
    mulsd xmm1, [hidden_w8]
    addsd xmm0, xmm1
    addsd xmm0, [hidden_b4]
    call relu
    movsd [hidden_out4], xmm0
    
    ; Calculate output
    movsd xmm0, [hidden_out1]
    mulsd xmm0, [output_w1]
    movsd xmm1, [hidden_out2]
    mulsd xmm1, [output_w2]
    addsd xmm0, xmm1
    movsd xmm1, [hidden_out3]
    mulsd xmm1, [output_w3]
    addsd xmm0, xmm1
    movsd xmm1, [hidden_out4]
    mulsd xmm1, [output_w4]
    addsd xmm0, xmm1
    addsd xmm0, [output_bias]
    call sigmoid
    movsd [output], xmm0
    
    leave
    ret

backward_pass:
    push rbp
    mov rbp, rsp
    
    ; Calculate output error
    movsd xmm0, [output]
    movsd xmm1, [target]
    subsd xmm1, xmm0
    movsd [error], xmm1
    
    ; Calculate output delta
    call sigmoid_derivative
    mulsd xmm0, [error]
    movsd [output_delta], xmm0
    
    ; Calculate hidden layer deltas
    ; Hidden neuron 1
    movsd xmm0, [output_delta]
    mulsd xmm0, [output_w1]
    movsd xmm1, [hidden_out1]
    call relu_derivative
    mulsd xmm0, xmm1
    movsd [hidden_delta1], xmm0
    
    ; Hidden neuron 2
    movsd xmm0, [output_delta]
    mulsd xmm0, [output_w2]
    movsd xmm1, [hidden_out2]
    call relu_derivative
    mulsd xmm0, xmm1
    movsd [hidden_delta2], xmm0
    
    ; Hidden neuron 3
    movsd xmm0, [output_delta]
    mulsd xmm0, [output_w3]
    movsd xmm1, [hidden_out3]
    call relu_derivative
    mulsd xmm0, xmm1
    movsd [hidden_delta3], xmm0
    
    ; Hidden neuron 4
    movsd xmm0, [output_delta]
    mulsd xmm0, [output_w4]
    movsd xmm1, [hidden_out4]
    call relu_derivative
    mulsd xmm0, xmm1
    movsd [hidden_delta4], xmm0
    
    leave
    ret

update_weights:
    push rbp
    mov rbp, rsp
    
    ; Update output weights
    movsd xmm0, [learning_rate]
    mulsd xmm0, [output_delta]
    
    ; Update output weight 1
    movsd xmm1, [hidden_out1]
    mulsd xmm1, xmm0
    addsd xmm1, [output_w1]
    movsd [output_w1], xmm1
    
    ; Update output weight 2
    movsd xmm1, [hidden_out2]
    mulsd xmm1, xmm0
    addsd xmm1, [output_w2]
    movsd [output_w2], xmm1
    
    ; Update output weight 3
    movsd xmm1, [hidden_out3]
    mulsd xmm1, xmm0
    addsd xmm1, [output_w3]
    movsd [output_w3], xmm1
    
    ; Update output weight 4
    movsd xmm1, [hidden_out4]
    mulsd xmm1, xmm0
    addsd xmm1, [output_w4]
    movsd [output_w4], xmm1
    
    ; Update output bias
    addsd xmm0, [output_bias]
    movsd [output_bias], xmm0
    
    ; Update hidden weights
    ; First hidden neuron
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta1]
    
    movsd xmm1, [input1]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w1]
    movsd [hidden_w1], xmm1
    
    movsd xmm1, [input2]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w2]
    movsd [hidden_w2], xmm1
    
    ; Second hidden neuron
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta2]
    
    movsd xmm1, [input1]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w3]
    movsd [hidden_w3], xmm1
    
    movsd xmm1, [input2]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w4]
    movsd [hidden_w4], xmm1
    
    ; Third hidden neuron
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta3]
    
    movsd xmm1, [input1]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w5]
    movsd [hidden_w5], xmm1
    
    movsd xmm1, [input2]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w6]
    movsd [hidden_w6], xmm1
    
    ; Fourth hidden neuron
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta4]
    
    movsd xmm1, [input1]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w7]
    movsd [hidden_w7], xmm1
    
    movsd xmm1, [input2]
    mulsd xmm1, xmm0
    addsd xmm1, [hidden_w8]
    movsd [hidden_w8], xmm1
    
    ; Update hidden biases
    movsd xmm0, [learning_rate]
    
    mulsd xmm0, [hidden_delta1]
    addsd xmm0, [hidden_b1]
    movsd [hidden_b1], xmm0
    
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta2]
    addsd xmm0, [hidden_b2]
    movsd [hidden_b2], xmm0
    
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta3]
    addsd xmm0, [hidden_b3]
    movsd [hidden_b3], xmm0
    
    movsd xmm0, [learning_rate]
    mulsd xmm0, [hidden_delta4]
    addsd xmm0, [hidden_b4]
    movsd [hidden_b4], xmm0
    
    leave
    ret

relu:
    ; Input in xmm0, output in xmm0
    xorpd xmm1, xmm1
    maxsd xmm0, xmm1
    ret

relu_derivative:
    ; Input in xmm0, output in xmm1
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    ja .positive
    xorpd xmm1, xmm1
    ret
.positive:
    movsd xmm1, [one]
    ret

sigmoid:
    ; Input in xmm0, output in xmm0
    push rbp
    mov rbp, rsp
    
    ; Calculate sigmoid: 1 / (1 + e^-x)
    movsd xmm1, [one]
    addsd xmm0, xmm1    ; 1 + x
    movsd xmm1, [one]
    divsd xmm1, xmm0    ; 1 / (1 + x)
    movsd xmm0, xmm1
    
    leave
    ret

sigmoid_derivative:
    ; Input in xmm0, output in xmm0
    push rbp
    mov rbp, rsp
    
    ; s * (1 - s)
    movsd xmm1, [one]
    subsd xmm1, xmm0
    mulsd xmm0, xmm1
    
    leave
    ret

usage_error:
    mov rdi, fmt_usage
    mov rsi, [r12]      ; Program name
    xor eax, eax
    call printf
    mov edi, 1
    call exit