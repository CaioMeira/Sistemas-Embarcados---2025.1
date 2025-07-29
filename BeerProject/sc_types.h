#ifndef SC_TYPES_H_
#define SC_TYPES_H_

#include <stdint.h>
#include <stddef.h>   // Para intptr_t
#include <stdbool.h>
#include <string>

namespace sc {

    typedef uint_fast16_t  ushort;
    typedef int32_t        integer;
    typedef int16_t        errorCode;
    typedef double         real;

    typedef int32_t        time;

    typedef uint_fast16_t  eventid;

    typedef bool           boolean;

    // Ponteiro nulo para uso no projeto
    constexpr auto sc_null = nullptr;

} // namespace sc

// Definições globais para facilitar uso em código que espera esses nomes
typedef sc::integer   sc_integer;
typedef sc::eventid   sc_eventid;
typedef sc::boolean   sc_boolean;
typedef sc::time      sc_time;

#define sc_null sc::sc_null

#endif /* SC_TYPES_H_ */
