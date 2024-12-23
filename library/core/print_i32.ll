; print_i32.ll
define void @print_i32(i32 %num) {
entry:
  ; Convert the integer to a string (simplified)
  %str = call i8* @itoa(i32 %num)  ; Convert integer to string

  ; File descriptor for stdout (1 for standard output)
  %fd = i32 1

  ; Calculate the length of the string
  %len = call i32 @strlen(i8* %str)  ; Get string length

  ; Perform the write system call (write(fd, str, len))
  ; syscall: write(fd, buffer, size)
  call i32 @write(i32 %fd, i8* %str, i32 %len)

  ret void
}

; Converts an integer to a null-terminated string
define i8* @itoa(i32 %num) {
entry:
  ; Allocate space for the string
  %buf = alloca [12 x i8]  ; Buffer for a 32-bit integer (enough for sign, digits, and null terminator)
  %ptr = getelementptr [12 x i8], [12 x i8]* %buf, i32 0, i32 0

  ; Handle zero case
  %zero_check = icmp eq i32 %num, 0
  br i1 %zero_check, label %zero_case, label %non_zero_case

zero_case:
  store i8 48, i8* %ptr  ; ASCII '0'
  ret i8* %ptr

non_zero_case:
  ; Convert the number to string (this example uses a simplified approach)
  ; Temporary variables for processing the digits
  %tmp_num = %num
  %digit = alloca i8
  %i = alloca i32
  store i32 0, i32* %i
  store i32 %num, i32* %tmp_num

  ; Loop to extract digits
  br label %loop_condition

loop_condition:
  %current_num = load i32, i32* %tmp_num
  %digit_value = srem i32 %current_num, 10  ; Get last digit
  store i32 %digit_value, i32* %digit

  ; Convert to ASCII
  %digit_ascii = add i32 %digit_value, 48  ; ASCII '0' is 48
  store i32 %digit_ascii, i32* %digit

  ; Update the pointer to move through the buffer
  %current_ptr = load i8*, i8** %ptr
  store i8 %digit_ascii, i8* %current_ptr

  ; Update number and pointer
  %next_num = sdiv i32 %current_num, 10  ; Divide number by 10
  store i32 %next_num, i32* %tmp_num
  %next_ptr = getelementptr i8, i8* %current_ptr, i32 1
  store i8* %next_ptr, i8** %ptr

  ; Check if the number is zero and exit loop if done
  %is_done = icmp eq i32 %next_num, 0
  br i1 %is_done, label %done, label %loop_condition

done:
  ; Null-terminate the string
  %null_terminator = getelementptr i8, i8* %ptr, i32 1
  store i8 0, i8* %null_terminator

  ret i8* %ptr
}

; Get the length of the string (excluding null terminator)
declare i32 @strlen(i8*)  ; strlen(buffer)

; Declare the write syscall (write(fd, buffer, size))
declare i32 @write(i32, i8*, i32)  ; write(fd, buffer, size)
