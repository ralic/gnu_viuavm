/*
 *  Copyright (C) 2015, 2016 Marek Marecki
 *
 *  This file is part of Viua VM.
 *
 *  Viua VM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Viua VM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Viua VM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <viua/types/type.h>
#include <viua/types/string.h>
#include <viua/types/vector.h>
#include <viua/types/exception.h>
#include <viua/cpu/frame.h>
#include <viua/cpu/registerset.h>
#include <viua/include/module.h>
using namespace std;


void io_getline(Frame* frame, RegisterSet*, RegisterSet*, Process*, CPU*) {
    string line;
    getline(cin, line);
    frame->regset->set(0, new String(line));
}


void io_readtext(Frame* frame, RegisterSet*, RegisterSet*, Process*, CPU*) {
    string path = frame->args->get(0)->str();
    ifstream in(path);

    ostringstream oss;
    string line;
    while (getline(in, line)) {
        oss << line << '\n';
    }

    frame->regset->set(0, new String(oss.str()));
}

const ForeignFunctionSpec functions[] = {
    { "std::io::getline/0", &io_getline },
    { "std::io::readtext/1", &io_readtext },
    { NULL, NULL },
};

extern "C" const ForeignFunctionSpec* exports() {
    return functions;
}
