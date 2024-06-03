### Begin with functions/executable code in the assmebly file via '.text' directive
.text
.global  set_temp_from_ports
        
## ENTRY POINT FOR REQUIRED FUNCTION
set_temp_from_ports:
### args are
### %rdi = temp_t *temp
### return value is an int, %rax
        ## assembly instructions here
        ## a useful technique for this problem
        #movX    SOME_GLOBAL_VAR(%rip), %reg
        # load global variable into register
        # Check the C type of the variable
        #    char / short / int / long
        # and use one of
        #    movb / movw / movl / movq 
        # and appropriately sized destination register                                            

        ## DON'T FORGET TO RETURN FROM FUNCTIONS

        movw THERMO_SENSOR_PORT(%rip), %dx   # copy global var to reg dx  (16-bit word)
        movb THERMO_STATUS_PORT(%rip), %cl   # copy global var to reg cl  (8-bit byte)
        
        ## check for invalid input
        cmpw $0, %dx # check if the sernsor port is lower than 0
        jl .INVALID # if so, jump to INVALID
        cmpw $28800, %dx # check if the sernsor port is greater than 28800
        jg .INVALID # if so, jump to INVALID
        testb $0b100, %cl # check if the status port is invalid
        jnz .INVALID # if so, jump to INVALID
        
        ## input is valid
        testw $0b10000, %dx # if the remainder >= 16, round up
        jz .SET_TEMPS # if no need for rounding, jump to setting the temps
        # round up
        movw %dx, %ax
        and $31, %ax # temp_degrees % 32 using masking
        addw $32, %dx # temp_degrees + 32
        subw %ax, %dx # temp_degrees - (temp_degrees % 32)
        
.SET_TEMPS: # set the temps
        sarw $5, %dx # divide sensor units by 32 via shifting
        subw $450, %dx # get the number of tenth degrees C above -45.0 C
        testb $0b100000, %cl # if port is C mode
        jz .C_MODE # jump to set the temp mode to C

        ## if port is in F mode, convert to F
        imul $9, %dx # multiply the temps by 9
        ## set for division
        movw %dx, %ax
        cwtl # sign extend ax to long word
        cltq # sign extend eax to quad word
        cqto # sign extend rax to rdx
        movl $5, %esi # divisor
        idiv %esi # divide
        movw %ax, %dx # load the quotient into %dx
        addw $320, %dx # add 320
        movb $2, 2(%rdi) # set temp mode to F
        jmp .FUNC_END # jump to the end of the func
        
.C_MODE:
        movb $1, 2(%rdi) # set temp mode to C
        jmp .FUNC_END # jump to the end of the func
        
.INVALID: # if the input is invalid
        movw $0, 0(%rdi) # temp->tenths_degrees = 0
        movb $3, 2(%rdi) # temp->temp_mode = 3
        movq $1, %rax # set the return value to 1
        ret

.FUNC_END:
        movw %dx, 0(%rdi) # set temp->tenths_degrees
        movq $0, %rax # set the return value to 0
        ret

### Change to definint semi-global variables used with the next function 
### via the '.data' directive
.data
	
my_int:                         # declare location an single integer named 'my_int'
        .int 1234               # value 1234

other_int:                      # declare another int accessible via name 'other_int'
        .int 0b0101             # binary value as per C

masks:                       # array of bit masks for 0-9
        .int 0b1111011  # 0              
        .int 0b1001000  # 1       
        .int 0b0111101  # 2      
        .int 0b1101101  # 3   
        .int 0b1001110  # 4   
        .int 0b1100111  # 5    
        .int 0b1110111  # 6   
        .int 0b1001001  # 7   
        .int 0b1111111  # 8    
        .int 0b1101111  # 9  

## WARNING: Don't forget to switch back to .text as below
## Otherwise you may get weird permission errors when executing 

.text
.global  set_display_from_temp

## ENTRY POINT FOR REQUIRED FUNCTION
set_display_from_temp:
### args are
### %rdi = temp_t temp
### %rsi = int *display
### return value is an int, %rax
        ## assembly instructions here
        ## two useful techniques for this problem
        ## movl    my_int(%rip),%eax    # load my_int into register eax
        ##leaq    my_array(%rip),%rdx  # load pointer to beginning of my_array into rdx

        leaq masks(%rip),%r8    # r8 points to array, rip used to enable relocation
        movl $0, (%rsi) # set display to 0

        ## check for invalid input
        movq %rdi, %rax
        shr $16, %rax # %al is the mode byte
        cmpb $1, %al ## if mode is C jump to .C_MODE_CHECK
        je .C_MODE_CHECK
        cmpb $2, %al ## if mode is F jump to .F_MODE
        je .F_MODE_CHECK
        jmp .INVALID_DISPLAY ## else jump to .INVALID_DISPLAY
        
.C_MODE_CHECK: # check if the degrees are in the valid C range
        cmpw $-450, %di #if tenths_degrees < -450
        jl .INVALID_DISPLAY # jump to .INVALID_DISPLAY
        cmpw $450, %di #if tenths_degrees > 450
        jg .INVALID_DISPLAY # jump to .INVALID_DISPLAY
        ## set the mode sign to C
        movl $1, %r9d
        shl $28, %r9d
        or %r9d, (%rsi)
        jmp .VALID # else jump to .VALID

.F_MODE_CHECK: # check if the degrees are in the valid F range
        cmpw $-490, %di #if tenths_degrees < -490
        jl .INVALID_DISPLAY # jump to .INVALID_DISPLAY
        cmpw $1130, %di #if tenths_degrees > 1130
        jg .INVALID_DISPLAY # jump to .INVALID_DISPLAY
        ## set the mode sign to F
        movl $1, %r9d
        shl $29, %r9d
        or %r9d, (%rsi)
        jmp .VALID # else jump to .VALID

.INVALID_DISPLAY: # invalid input
        movl $0b0110111, %r10d # set E
        shl $21, %r10d # shift left by 21 positions
        or %r10d, (%rsi) # 'or' the mask and the display
        movl $0b1011111, %r10d # set R
        shl $14, %r10d # shift left by 14 positions
        or %r10d, (%rsi) # 'or' the mask and the display
        movl $0b1011111, %r10d # set R
        shl $7, %r10d # shift left by 14 positions
        or %r10d, (%rsi) # 'or' the mask and the display
        movl $1, %eax # set return value to 1
        ret

.VALID: # input is valid
        movl $0, %ecx # %ecx = 0 represents temps are positive
        movw %di, %r11w # %r11w is the tenths degrees
        cmpw $0, %r11w # check if r11w is negative
        jg .POSITIVE # jump to .POSITIVE
        imul $-1, %r11w # %r11w is now positive
        movl $1, %ecx # %ecx = 1 represents that temps are negative

.POSITIVE: # tenths degrees are represented as positive, or were converted to positive
        ## set each digit from the temp
        ## tenths digit
        movw %r11w, %ax # set for divison
        cwtl # sign extend ax to long word
        cltq # sign extend eax to quad word
        cqto # sign extend rax to rdx
        movw $10, %r9w # divisor
        idiv %r9w # divide
        movq %rdx, %r9 # load the remainder into %r9w
        ## set the tenth digit of the display
        movl (%r8,%r9,4), %r10d
        or %r10d, (%rsi) # set the value into the display
        
        ## ones digit
        # set for divison by 100
        movw %r11w, %ax 
        cwtl # sign extend ax to long word
        cltq # sign extend eax to quad word
        cqto # sign extend rax to rdx
        movw $100, %r9w # divisor
        idiv %r9w # divide
        movw %dx, %r9w # load the remainder into %r9w
        # set for divison by 10
        movw %r9w, %ax
        cwtl # sign extend ax to long word
        cltq # sign extend eax to quad word
        cqto # sign extend rax to rdx
        movw $10, %r9w # divisor
        idiv %r9w # divide
        movq %rax, %r9 # load the quotient into %r9w
        # set the ones digit of the display
        movl (%r8,%r9,4), %r10d
        shl $7, %r10d # shift left by 7
        or %r10d, (%rsi) # set the value into the display
        

        ## tens digit
        movw %r11w, %ax # set for divison by 100
        cwtl # sign extend ax to long word
        cltq # sign extend eax to quad word
        cqto # sign extend rax to rdx
        movw $100, %r9w # divisor
        idiv %r9w # divide
        movq %rax, %r9 # load the quotient into %r9w
        # set the ones digit of the display
        cmpq $0, %r9
        je .TENS_DIGIT_IS_ZERO # if the tens digit is 0, jump
        cmpq $10, %r9
        jge .TENS_DIGIT_GE_10 # if the tens digit >= 10, jump
        # tens digit is less than 10
        movl (%r8,%r9,4), %r10d # get the mask[tens_value]
        shl $14, %r10 # mask[tens_value] << 14
        or %r10d, (%rsi) # set the value into the display
        cmpl $1, %ecx 
        jne .SECOND_FUNC_END # if the temps are non-negative, jump to the end of the func
        shl $23, %ecx # 1 << 23
        or %ecx, (%rsi) # set the minus into the display
        jmp .SECOND_FUNC_END # if the temps are non-negative, jump to the end of the func

.TENS_DIGIT_GE_10: # if the tens value is a double digit number
        ## set the leftmost digit to 1
        movq $1, %r10 # store 1 in %r10
        movl (%r8,%r10,4), %r10d # get the mask[1]
        shl $21, %r10 # mask[1] << 21
        or %r10d, (%rsi) # set the value into the display
        ## set the digit to the right
        subq $10, %r9 # sub 10 from the tens value
        movl (%r8,%r9,4), %r10d # get the mask[tens_value - 10]
        shl $14, %r10d # mask[tens_value - 10] << 14
        or %r10d, (%rsi) # set the value into the display
        jmp .SECOND_FUNC_END # if the temps are non-negative, jump to the end of the func
.TENS_DIGIT_IS_ZERO:
        cmpl $1, %ecx # check if the temps are negative
        jne .SECOND_FUNC_END # if the temps are non-negative, jump to the end of the func
        shl $16, %ecx # 1 << 16
        or %ecx, (%rsi) # set the minus into the display

.SECOND_FUNC_END:
        movq $0, %rax # set the return value to 0 and return
        ret

.text
.global thermo_update
        
## ENTRY POINT FOR REQUIRED FUNCTION
thermo_update:
	## assembly instructions here
        ## create a temp_r struct on the stack
        push THERMO_SENSOR_PORT(%rip) # push the THERMO_SENSOR_PORT onto the stack
        push THERMO_STATUS_PORT(%rip) # push the THERMO_STATUS_PORT onto the stack
        movq %rsp, %rdx # load the address of the beginning of the stack into %rdx
        movq %rdx, %rdi # load the address of the beginning of the stack from %rdx into the first func argument
        call set_temp_from_ports # call the first function
        push %rax # return value from the first function is stored on the stack

        subq $8, %rsp # allocate space for the second function call
        movq (%rdi), %rdi # deref the beginning of the stack from into the first func argument
        leaq THERMO_DISPLAY_PORT(%rip), %rsi # add the address of the display port from the stack into the first function argument
        call set_display_from_temp # call the second function
        addq $8, %rsp # dellocate space for the second function call

.END_OF_PROBLEM1:
        pop %r10 # pop the first return value
        or %r10, %rax ## store 0 in as the return value only if both functions returned 0
        pop %r10 # pop THERMO_STATUS_PORT 
        pop %r10 # pop THERMO_SENSOR_PORT
        ret 