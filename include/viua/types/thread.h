#ifndef VIUA_TYPES_STRING_H
#define VIUA_TYPES_STRING_H

#pragma once

#include <string>
#include "type.h"
#include "vector.h"
#include "integer.h"
#include "../support/string.h"
#include <viua/cpu/frame.h>
#include <viua/cpu/registerset.h>


// we only need a pointer so class declaration will be sufficient
class Thread;


class ThreadType : public Type {
    /** String type.
     *
     *  Designed to hold text.
     */
    Thread* thrd;

    public:
        std::string type() const;
        std::string str() const;
        std::string repr() const;
        bool boolean() const;
        ThreadType* copy() const;

        virtual void joinable(Frame*, RegisterSet*, RegisterSet*);
        virtual void join(Frame*, RegisterSet*, RegisterSet*);
        virtual void detach(Frame*, RegisterSet*, RegisterSet*);

        ThreadType(Thread* t): thrd(t) {}
};


#endif
