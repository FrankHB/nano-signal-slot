#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <random>
#include <numeric>
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

template<typename T, class R, typename FI>
void
sequence_shuffle(FI first, FI last, R& rng, T val = 0)
{
    std::iota(first, last, val);
    std::shuffle(first, last, rng);
}

template<typename T, class R>
std::vector<T>
make_random_sequence(size_t n)
{
	R rng;
	std::vector<std::size_t> vec(n);

	sequence_shuffle<std::size_t>(vec.begin(), vec.end(), rng);
	return std::move(vec);
}

#endif // BENCHMARK_HPP
