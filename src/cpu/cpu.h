#ifndef TATANKA_CPU_H
#define TATANKA_CPU_H

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "../bytecode/bytetypedef.h"
#include "../types/object.h"
#include "frame.h"


const int DEFAULT_REGISTER_SIZE = 256;


class CPU {
    /*  Bytecode pointer is a pointer to program's code.
     *  Size and executable offset are metadata exported from bytecode dump.
     */
    byte* bytecode;
    uint16_t bytecode_size;
    uint16_t executable_offset;

    /*  Registers and their number stored.
     */
    Object** registers;
    bool* references;
    int reg_count;

    // Currently used register set
    Object** uregisters;
    bool* ureferences;
    int uregisters_size;

    /*  Call stack.
     */
    std::vector<Frame*> frames;
    Frame* frame_new;

    /*  Function ID to bytecode address map.
     */
    std::map<const char*, int> function_addresses;

    /*  Methods to deal with registers.
     */
    void updaterefs(Object* before, Object* now);
    bool hasrefs(int index);
    Object* fetch(int);
    void place(int, Object*);

    /*  Methods implementing CPU instructions.
     */
    byte* istore(byte*);
    byte* iadd(byte*);
    byte* isub(byte*);
    byte* imul(byte*);
    byte* idiv(byte*);

    byte* ilt(byte*);
    byte* ilte(byte*);
    byte* igt(byte*);
    byte* igte(byte*);
    byte* ieq(byte*);

    byte* iinc(byte*);
    byte* idec(byte*);

    byte* fstore(byte*);
    byte* fadd(byte*);
    byte* fsub(byte*);
    byte* fmul(byte*);
    byte* fdiv(byte*);

    byte* flt(byte*);
    byte* flte(byte*);
    byte* fgt(byte*);
    byte* fgte(byte*);
    byte* feq(byte*);

    byte* bstore(byte*);

    byte* boolean(byte*);
    byte* lognot(byte*);
    byte* logand(byte*);
    byte* logor(byte*);

    byte* move(byte*);
    byte* copy(byte*);
    byte* ref(byte*);
    byte* swap(byte*);
    byte* del(byte*);
    byte* isnull(byte*);

    byte* ret(byte*);

    byte* print(byte*);
    byte* echo(byte*);

    byte* frame(byte*);
    byte* param(byte*);
    byte* paref(byte*);
    byte* arg(byte*);

    byte* call(byte*);
    byte* end(byte*);

    byte* jump(byte*);
    byte* branch(byte*);

    public:
        // debug flag
        bool debug;

        /*  Public API of the CPU provides basic actions:
         *
         *      * load bytecode,
         *      * set its size,
         *      * tell the CPU where to start execution,
         *      * kick the CPU so it starts running,
         */
        CPU& load(byte*);
        CPU& bytes(uint16_t);
        CPU& eoffset(uint16_t);

        CPU& mapfunction(const char*, int);

        int run();

        CPU(int r = DEFAULT_REGISTER_SIZE): bytecode(0), bytecode_size(0), executable_offset(0), registers(0), references(0), reg_count(r),
                                            uregisters(0), ureferences(0), uregisters_size(0), frame_new(0), debug(false) {
            /*  Basic constructor.
             *  Creates registers array of requested size and
             *  initializes it with zeroes.
             */
            registers = new Object*[reg_count];
            references = new bool[reg_count];
            for (int i = 0; i < reg_count; ++i) {
                registers[i] = 0;
                references[i] = false;
            }
            uregisters = registers;
            ureferences = references;
            uregisters_size = reg_count;
        }

        ~CPU() {
            /*  Destructor must free all memory allocated for values stored in registers.
             *  Here we iterate over all registers and delete non-null pointers.
             *
             *  Destructor also frees memory at bytecode pointer so make sure you gave CPU a copy of the bytecode if you want to keep it
             *  after the CPU is finished.
             */
            for (int i = 0; i < reg_count; ++i) {
                if (registers[i] and !references[i]) {
                    delete registers[i];
                }
            }
            delete[] registers;
            delete[] references;
            if (bytecode) { delete[] bytecode; }
        }
};

#endif
