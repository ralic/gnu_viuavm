#ifndef VIUA_CPU_H
#define VIUA_CPU_H

#pragma once

#include <dlfcn.h>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <viua/thread.h>


class CPU {
    friend Thread;
#ifdef AS_DEBUG_HEADER
    public:
#endif
    /*  Bytecode pointer is a pointer to program's code.
     *  Size and executable offset are metadata exported from bytecode dump.
     */
    byte* bytecode;
    uint64_t bytecode_size;
    uint64_t executable_offset;

    // vector of all threads machine is executing
    std::vector<Thread> threads;

    // Global register set
    RegisterSet* regset;
    // Currently used register set
    RegisterSet* uregset;

    // Temporary register
    Type* tmp;

    // Static registers
    std::map<std::string, RegisterSet*> static_registers;

    // Map of the typesystem currently existing inside the VM.
    std::map<std::string, Prototype*> typesystem;

    /*  Call stack.
     */
    std::vector<Frame*> frames;
    Frame* frame_new;

    /*  Block stack.
     */
    std::vector<TryFrame*> tryframes;
    TryFrame* try_frame_new;

    /*  Function and block names mapped to bytecode addresses.
     */
    byte* jump_base;
    std::map<std::string, uint64_t> function_addresses;
    std::map<std::string, uint64_t> block_addresses;

    std::map<std::string, std::pair<std::string, byte*>> linked_functions;
    std::map<std::string, std::pair<std::string, byte*>> linked_blocks;
    std::map<std::string, std::pair<unsigned, byte*> > linked_modules;

    /*  Slot for thrown objects (typically exceptions).
     *  Can be set by user code and the CPU.
     */
    Type* thrown;
    Type* caught;

    /*  Variables set after CPU executed bytecode.
     *  They describe exit conditions of the bytecode that just stopped running.
     */
    int return_code;                // always set
    std::string return_exception;   // set if CPU stopped because of an exception
    std::string return_message;     // message set by exception

    uint64_t instruction_counter;
    byte* instruction_pointer;

    /*  This is the interface between programs compiled to VM bytecode and
     *  extension libraries written in C++.
     */
    std::map<std::string, ExternalFunction*> foreign_functions;

    /** This is the mapping Viua uses to dispatch methods on pure-C++ classes.
     */
    std::map<std::string, ForeignMethod> foreign_methods;

    /*  Methods to deal with registers.
     */
    void updaterefs(Type* before, Type* now);
    bool hasrefs(unsigned);
    Type* fetch(unsigned) const;
    void place(unsigned, Type*);
    void ensureStaticRegisters(std::string);

    /*  Methods dealing with stack and frame manipulation, and
     *  function calls.
     */
    Frame* requestNewFrame(int arguments_size = 0, int registers_size = 0);
    TryFrame* requestNewTryFrame();
    void pushFrame();
    void dropFrame();
    // call native (i.e. written in Viua) function
    byte* callNative(byte*, const std::string&, const bool&, const int&, const std::string&);
    // call foreign (i.e. from a C++ extension) function
    byte* callForeign(byte*, const std::string&, const bool&, const int&, const std::string&);
    // call foreign method (i.e. method of a pure-C++ class loaded into machine's typesystem)
    byte* callForeignMethod(byte*, Type*, const std::string&, const bool&, const int&, const std::string&);

    /*  Methods dealing with dynamic library loading.
     */
    std::vector<void*> cxx_dynamic_lib_handles;
    void loadNativeLibrary(const std::string&);
    void loadForeignLibrary(const std::string&);

    /*  Methods dealing with typesystem related tasks.
     */
    std::vector<std::string> inheritanceChainOf(const std::string&);

    public:
        // debug and error reporting flags
        bool debug, errors;

        std::vector<std::string> commandline_arguments;

        /*  Public API of the CPU provides basic actions:
         *
         *      * load bytecode,
         *      * set its size,
         *      * tell the CPU where to start execution,
         *      * kick the CPU so it starts running,
         */
        CPU& load(byte*);
        CPU& bytes(uint64_t);
        CPU& eoffset(uint64_t);
        CPU& preload();

        CPU& mapfunction(const std::string&, uint64_t);
        CPU& mapblock(const std::string&, uint64_t);

        CPU& registerExternalFunction(const std::string&, ExternalFunction*);
        CPU& removeExternalFunction(std::string);

        /// These two methods are used to inject pure-C++ classes into machine's typesystem.
        CPU& registerForeignPrototype(const std::string&, Prototype*);
        CPU& registerForeignMethod(const std::string&, ForeignMethod);

        byte* begin();
        inline byte* end() { return nullptr; }

        CPU& iframe(Frame* frm = nullptr, unsigned r = DEFAULT_REGISTER_SIZE);

        byte* tick();

        void cycle();

        int run();
        inline decltype(instruction_counter) counter() { return instruction_counter; }

        inline std::tuple<int, std::string, std::string> exitcondition() {
            return std::tuple<int, std::string, std::string>(return_code, return_exception, return_message);
        }
        inline std::vector<Frame*> trace() { return frames; }

        CPU():
            bytecode(nullptr), bytecode_size(0), executable_offset(0),
            regset(nullptr), uregset(nullptr),
            tmp(nullptr),
            static_registers({}),
            frame_new(nullptr),
            try_frame_new(nullptr),
            jump_base(nullptr),
            thrown(nullptr), caught(nullptr),
            return_code(0), return_exception(""), return_message(""),
            instruction_counter(0), instruction_pointer(nullptr),
            debug(false), errors(false)
        {}

        ~CPU() {
            /*  Destructor frees memory at bytecode pointer so make sure you passed a copy of the bytecode to the constructor
             *  if you want to keep it around after the CPU is finished.
             */
            if (bytecode) { delete[] bytecode; }

            std::map<std::string, RegisterSet*>::iterator sr = static_registers.begin();
            while (sr != static_registers.end()) {
                std::string  rkey = sr->first;
                RegisterSet* rset = sr->second;

                ++sr;

                static_registers.erase(rkey);
                delete rset;
            }

            std::map<std::string, std::pair<unsigned, byte*> >::iterator lm = linked_modules.begin();
            while (lm != linked_modules.end()) {
                std::string lkey = lm->first;
                byte *ptr = lm->second.second;

                ++lm;

                linked_modules.erase(lkey);
                delete[] ptr;
            }

            std::map<std::string, Prototype*>::iterator pr = typesystem.begin();
            while (pr != typesystem.end()) {
                std::string proto_name = pr->first;
                Prototype* proto_ptr = pr->second;

                ++pr;

                typesystem.erase(proto_name);
                delete proto_ptr;
            }

            for (unsigned i = 0; i < cxx_dynamic_lib_handles.size(); ++i) {
                dlclose(cxx_dynamic_lib_handles[i]);
            }
        }
};

#endif
