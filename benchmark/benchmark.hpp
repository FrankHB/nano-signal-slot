#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>
#include <thread>

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

class chrono_timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

    public:

    void reset()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    template <typename T>
    static void delay(std::size_t duration)
    {
        std::this_thread::sleep_for(T(duration));
    }
    template <typename T>
    std::size_t count() const
    {
        return std::chrono::duration_cast<T>
          (std::chrono::high_resolution_clock::now() - m_start).count();
    }
};

// Functors
struct IncrementFill
{
    std::size_t i;
    IncrementFill ( ) : i(0) { }
    std::size_t operator()() { return i++; }
};

#endif // BENCHMARK_HPP
