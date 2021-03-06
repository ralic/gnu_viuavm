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
#include <viua/bytecode/bytetypedef.h>
#include <viua/types/integer.h>
#include <viua/types/pointer.h>
#include <viua/types/object.h>
#include <viua/types/string.h>
#include <viua/cpu/opex.h>
#include <viua/exceptions.h>
#include <viua/cpu/registerset.h>
#include <viua/operand.h>
#include <viua/assert.h>
#include <viua/cpu/cpu.h>
#include <viua/scheduler/vps.h>
using namespace std;


byte* Process::opnew(byte* addr) {
    /** Create new instance of specified class.
     */
    unsigned target = viua::operand::getRegisterIndex(viua::operand::extract(addr).get(), this);

    string class_name = viua::operand::extractString(addr);

    if (not scheduler->isClass(class_name)) {
        throw new Exception("cannot create new instance of unregistered type: " + class_name);
    }

    place(target, new Object(class_name));

    return addr;
}

byte* Process::opmsg(byte* addr) {
    /** Send a message to an object.
     *
     *  This instruction is used to perform a method call on an object using dynamic dispatch.
     *  To call a method using static dispatch (where a correct function is resolved during compilation) use
     *  "call" instruction.
     */
    int return_register_index;
    bool return_register_ref = false;

    viua::cpu::util::extractIntegerOperand(addr, return_register_ref, return_register_index);

    if (return_register_ref) {
        // FIXME: remove the need for static_cast<>
        // the cast is safe because register indexes cannot be negative, but it looks ugly
        return_register_index = static_cast<Integer*>(fetch(static_cast<unsigned>(return_register_index)))->value();
    }

    string method_name = viua::operand::extractString(addr);

    Type* obj = frame_new->args->at(0);
    if (Pointer* ptr = dynamic_cast<Pointer*>(obj)) {
        obj = ptr->to();
    }
    if (not scheduler->isClass(obj->type())) {
        throw new Exception("unregistered type cannot be used for dynamic dispatch: " + obj->type());
    }
    vector<string> mro = scheduler->inheritanceChainOf(obj->type());
    mro.insert(mro.begin(), obj->type());

    string function_name = "";
    for (unsigned i = 0; i < mro.size(); ++i) {
        if (not scheduler->isClass(mro[i])) {
            throw new Exception("unavailable base type in inheritance hierarchy of " + mro[0] + ": " + mro[i]);
        }
        if (scheduler->classAccepts(mro[i], method_name)) {
            function_name = scheduler->resolveMethodName(mro[i], method_name);
            break;
        }
    }
    if (function_name.size() == 0) {
        throw new Exception("class '" + obj->type() + "' does not accept method '" + method_name + "'");
    }

    bool is_native = scheduler->isNativeFunction(function_name);
    bool is_foreign = scheduler->isForeignFunction(function_name);
    bool is_foreign_method = scheduler->isForeignMethod(function_name);

    if (not (is_native or is_foreign or is_foreign_method)) {
        throw new Exception("method '" + method_name + "' resolves to undefined function '" + function_name + "' on class '" + obj->type() + "'");
    }

    if (is_foreign_method) {
        // FIXME: remove the need for static_cast<>
        // the cast is safe because register indexes cannot be negative, but it looks ugly
        return callForeignMethod(addr, obj, function_name, return_register_ref, static_cast<unsigned>(return_register_index), method_name);
    }

    auto caller = (is_native ? &Process::callNative : &Process::callForeign);
    // FIXME: remove the need for static_cast<>
    // the cast is safe because register indexes cannot be negative, but it looks ugly
    return (this->*caller)(addr, function_name, return_register_ref, static_cast<unsigned>(return_register_index), method_name);
}

byte* Process::opinsert(byte* addr) {
    /** Insert an object as an attribute of another object.
     */
    Type* object_operand = viua::operand::extract(addr)->resolve(this);
    Type* key_operand = viua::operand::extract(addr)->resolve(this);
    unsigned source_index = viua::operand::getRegisterIndex(viua::operand::extract(addr).get(), this);

    viua::assertions::assert_implements<Object>(object_operand, "Object");
    viua::assertions::assert_typeof(key_operand, "String");

    static_cast<Object*>(object_operand)->insert(static_cast<String*>(key_operand)->str(), pop(source_index));

    return addr;
}

byte* Process::opremove(byte* addr) {
    /** Remove an attribute of another object.
     */
    unsigned target_index = viua::operand::getRegisterIndex(viua::operand::extract(addr).get(), this);
    Type* object_operand = viua::operand::extract(addr)->resolve(this);
    Type* key_operand = viua::operand::extract(addr)->resolve(this);

    viua::assertions::assert_implements<Object>(object_operand, "Object");
    viua::assertions::assert_typeof(key_operand, "String");

    place(target_index, static_cast<Object*>(object_operand)->remove(static_cast<String*>(key_operand)->str()));

    return addr;
}
