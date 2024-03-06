.globl main
        main: 

        # prologue
        pushq %rbp 
        movq %rsp, %rbp 

        # body
        movl $5, -8(%rbp)
        movl -8(%rbp), %eax
        movl %eax, -4(%rbp)
        movl $7, -16(%rbp)
        movl -16(%rbp), %eax
        mul -4(%rbp)
        movl %eax, -20(%rbp)
        movl -20(%rbp), %eax
        movl %eax, -12(%rbp)
        movl -12(%rbp), %eax

        # epilogue
        popq %rbp
        ret