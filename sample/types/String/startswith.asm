.function: main
    frame ^[(param 0 (strstore 1 "Hello World!")) (param 1 (strstore 2 "Hello"))]
    print (msg 3 startswith)

    frame ^[(param 0 1) (param 1 (strstore 2 "Fail"))]
    print (msg 3 startswith)

    izero 0
    end
.end