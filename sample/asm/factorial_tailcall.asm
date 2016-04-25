.function: factorial
    .name: 1 number
    .name: 0 result
    ; multiply "result" (reference) by "number" (copy)
    ; and store the resulting integer in "result"
    ; calculation is available outside of the local scope
    imul result (arg result 1) (arg number 0)
    idec number

    ; if counter is equal to zero
    ; finish "factorial" calls
    branch (ieq 4 number (istore 3 0)) finish

    ; this frame must be the same as in "main"
    ; result must still be a reference
    frame ^[(pamv 0 number) (pamv 1 result)]
    tailcall factorial

    .mark: finish
    return
.end

.function: main
    .name: 1 number
    .name: 2 result
    ; store the number of which we want to calculate the factorial
    istore number 8

    ; create frame for two parameters:
    ; * first is a copy of the number
    ; * second is a reference to result register
    ;   because we want to display it here, after calls to factorial are finished
    frame ^[(param 0 number) (pamv 1 (istore result 1))]
    call result factorial

    ; print result
    print result
    izero 0
    return
.end