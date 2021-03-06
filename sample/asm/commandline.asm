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

.function: main/1
    .name: 2 len
    .name: 3 counter

    vlen len (arg 1 0)
    istore counter 0

    .mark: loop
    branch (ilt 4 counter len) +1 break

    print (vat 5 1 @counter)

    iinc counter
    jump loop

    .mark: break

    izero 0
    return
.end
