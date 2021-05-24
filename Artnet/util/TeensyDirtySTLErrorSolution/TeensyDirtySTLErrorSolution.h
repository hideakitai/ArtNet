#pragma once

#include <Arduino.h>

#ifdef TEENSYDUINO
#ifndef TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION
#define TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION


#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) // TEENSY 3.1/3.2, 3.5, 3.6

extern "C"
{
    int _write() { return -1; }
}

#elif defined(__IMXRT1062__) // TEENSY4.0

extern "C"
{
    void *__exidx_start __attribute__((__visibility__ ("hidden")));
    void *__exidx_end __attribute__((__visibility__ ("hidden")));
}

#else

    #error NOT SUPPORTED TEENSY VERSION

#endif


// ----- template -----
//
// #if defined(__MK20DX256__) // TEENSY3.1/3.2
// #elif defined(__MK64FX512__) // TEENSY3.5
// #elif defined(__MK66FX1M0__) // TEENSY3.6
// #elif defined(__IMXRT1062__) // TEENSY4.0
//
// extern "C"
// {
//     int _getpid() { return -1; }
//     int _kill(int pid, int sig) { return -1; }
//     int _write() { return -1; }
//     void *__exidx_start __attribute__((__visibility__ ("hidden")));
//     void *__exidx_end __attribute__((__visibility__ ("hidden")));
//     int _getpid();// { return -1; }
//     int _kill(int pid, int sig);// { return -1; }
//     int _write() { return -1; }
//     void *__exidx_start __attribute__((__visibility__ ("hidden")));
//     void *__exidx_end __attribute__((__visibility__ ("hidden")));
// }
//
// // copied from https://github.com/gcc-mirror/
// namespace std
// {
//     void __throw_bad_alloc() { _GLIBCXX_THROW_OR_ABORT(bad_alloc()); }
//     void __throw_length_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(length_error(_(__s))); }
//     void __throw_bad_function_call() { _GLIBCXX_THROW_OR_ABORT(bad_function_call()); }
//     void _Rb_tree_decrement(std::_Rb_tree_node_base* a) {}
//     void _Rb_tree_insert_and_rebalance(bool, std::_Rb_tree_node_base*, std::_Rb_tree_node_base*, std::_Rb_tree_node_base&) {}
// }

#endif // TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION
#endif // TEENSYDUINO

