.function: by_reference
    arg 0 0
    end
.end

.function: main
    ; store a number in register
    istore 1 69

    ; pass it by reference to the function
    frame ^[(paref 0 1)]

    ; store return value in another register (it is a reference!)
    call 2 by_reference

    ; assign different value to it
    istore 2 42

    ; check if return-by-reference is working (should print 42)
    print 1

    izero 0
    end
.end
