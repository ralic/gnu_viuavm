.function: is_not_negative
    arg 1 0
    igte 0 1 (izero 2)
    return
.end

.signature: std::vector::every/2
.signature: std::vector::of_ints/1

.function: main
    link std::vector

    frame ^[(param 0 (istore 1 20))]
    call 2 std::vector::of_ints/1

    frame ^[(param 0 2) (pamv 1 (function 4 is_not_negative))]
    call 5 std::vector::every/2
    print 5

    izero 0
    return
.end