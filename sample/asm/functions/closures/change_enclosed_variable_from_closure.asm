;
;   Copyright (C) 2015, 2016 Marek Marecki
;
;   This file is part of Viua VM.
;
;   Viua VM is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.
;
;   Viua VM is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with Viua VM.  If not, see <http://www.gnu.org/licenses/>.
;

.function: variable_changing_function/0
    ; expects register 1 to be an enclosed object
    istore 1 42
    return
.end

.function: main/1
    ; create a closure and enclose object in register 1 with it
    closure 2 variable_changing_function/0
    enclose 2 1 (strstore 1 "Hello World!")

    ; should print "Hello World!"
    print 1

    ; call the closure
    frame 0
    fcall 0 2

    ; should print "42"
    print 1

    izero 0
    return
.end
