#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <random>

// Friendly macro definitions
#if _MSC_VER >= 1400
#define NOINLINE(s) __declspec(noinline) s
#else
#define NOINLINE(s) s __attribute__ ((noinline))
#endif

// Globals
extern std::size_t g_limit;
const std::size_t c_jlsignal_max = 9001;

// General typedefs
typedef std::minstd_rand Rng_t;
typedef std::uniform_int_distribution<std::size_t> Eng_t;

// Unit related typedefs
typedef std::chrono::seconds Limit_u;
typedef std::chrono::nanoseconds Timer_u;
typedef std::chrono::duration<double, std::milli> Delta_u;

// Functors
struct IncrementFill
{
    std::size_t i;
    IncrementFill ( ) : i(0) { }
    std::size_t operator()() { return i++; }
};

#endif // BENCHMARK_HPP
