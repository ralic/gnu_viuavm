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

.signature: throwing::oh_noes/0

.block: __try
    frame 0
    call 0 throwing::oh_noes/0
    leave
.end
.block: __catch_Exception
    print (pull 1)
    leave
.end

.function: watchdog_process/0
    .mark: start_watching
    .name: 1 death_message
    receive death_message

    .name: 3 exception
    remove exception death_message (strstore exception "exception")
    print exception

    jump start_watching
    return
.end

.function: main/1
    frame 0
    watchdog watchdog_process/0

    import "build/test/throwing"

    try
    catch "ExceptionX" __catch_Exception
    enter __try

    izero 0
    return
.end
