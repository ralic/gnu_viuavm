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
#include <string>
#include <sstream>
#include <viua/types/object.h>
#include <viua/types/exception.h>
#include <viua/cpu/frame.h>
using namespace std;


string Object::type() const {
    return type_name;
}
bool Object::boolean() const {
    return true;
}

Type* Object::copy() const {
    Object* cp = new Object(type_name);
    return cp;
}

void Object::set(const string& name, Type* object) {
    // prevent memory leaks during key overwriting
    if (attributes.count(name)) {
        delete attributes.at(name);
        attributes.erase(name);
    }
    attributes[name] = object;
}

void Object::insert(const string& key, Type* value) {
    set(key, value);
}
Type* Object::remove(const string& key) {
    if (not attributes.count(key)) {
        ostringstream oss;
        oss << "attribute not found: " << key;
        throw new Exception(oss.str());
    }
    Type* o = attributes.at(key);
    attributes.erase(key);
    return o;
}


Object::Object(const std::string& tn): type_name(tn) {}
Object::~Object() {
    auto kv_pair = attributes.begin();
    while (kv_pair != attributes.end()) {
        string key = kv_pair->first;
        Type* value = kv_pair->second;

        ++kv_pair;

        attributes.erase(key);
        delete value;
    }
}
