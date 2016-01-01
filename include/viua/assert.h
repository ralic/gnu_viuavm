#ifndef VIUA_ASSERT_H
#define VIUA_ASSERT_H

#pragma once

#include <string>
#include <viua/cpu/frame.h>
#include <viua/types/type.h>
#include <viua/exceptions.h>


namespace viua {
    namespace assertions {
        template<class T> void assert_implements(Type* object, const std::string& s) {
            /** Use this assertion when casting to interface type (C++ abstract class).
             *
             *  Example: casting Vector to Iterator.
             */
            if (dynamic_cast<T*>(object) == nullptr) {
                throw new Exception(object->type() + " does not implement " + s);
            }
        }

        void assert_typeof(Type* object, const std::string& expected);


        template<typename T, typename U> inline bool any_equal(const T& to_compare, const U& first) {
            return (to_compare == first);
        }
        template<typename T, typename U, typename... R> bool any_equal(const T& to_compare, const U& first, const R&... rest) {
            return ((to_compare == first) or any_equal(to_compare, rest...));
        }

        using Arity = size_t;

        template<typename... A> void assert_arity(const Frame* frame, const A&... valid_arities) {
            Arity arity = frame->args->size();
            if (not any_equal(arity, valid_arities...)) {
                throw new ArityException(arity, {valid_arities...});
            }
        }
    }
}



#endif