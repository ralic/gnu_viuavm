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

; this is the first program written for Viua VM that combines
; instructions to carry out an action that is not hardcoded as
; a standalone isntruction
;
; the purpose of this program is to find an absolute value of an integer

.function: main/1
    .name: 1 number
    .name: 2 zero
    .name: 3 is_negative

    ; store the int, of which we want to get an absolute value
    istore number -17

    ; if the int is less than zero, multiply it by -1
    ; else, branch directly to print instruction
    ; the negation of boolean is just to use short form of branch
    ; instruction - this construction starts emerging as a pattern...
    branch (not (ilt is_negative number (istore zero 0))) final_print
    imul 1 (istore 4 -1)

    .mark: final_print
    print number
    izero 0
    return
.end
