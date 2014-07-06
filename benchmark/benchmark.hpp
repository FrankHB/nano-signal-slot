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


namespace Benchmark
{

struct NoTrack
{};


template<typename F, typename... P>
static auto
emit(F&& f, P&&... args) -> decltype(static_cast<F&&>(f)(
	static_cast<P&&>(args)...))
{
	static_cast<F&&>(f)(static_cast<P&&>(args)...);
}
template<typename F, typename... P>
static auto
emit(F&& f, P&&... args) -> decltype(static_cast<F&&>(f).emit(
	static_cast<P&&>(args)...))
{
	static_cast<F&&>(f).emit(static_cast<P&&>(args)...);
}
template<typename T1, typename T2, typename... P>
static void
emit(std::pair<T1, T2>, P&&...)
{}
template<typename... P>
static void
emit(int, P&&...)
{}


template<class Subject, class Base = NoTrack>
class SignalSlotBenchmark : public Base
{
public:
	using Foo = Benchmark::SignalSlotBenchmark<Subject, Base>;
    static chrono_timer timer;

	NOINLINE(void handler(Rng_t& rng))
    {
        volatile std::size_t a = rng();

		static_cast<void>(a);
    }

	NOINLINE(static void do_connect(Subject& subject, Foo& foo))
	{
		connect(subject, &Foo::handler, foo);
	}

    NOINLINE(static double construction(std::size_t N))
    {
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            timer.reset();

			std::unique_ptr<Subject> subject(new Subject());
            std::vector<Foo> foo_array(N);

			do_connect(*subject, foo_array.back());
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

	NOINLINE(static double destruction(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N));
		Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            {
                std::unique_ptr<Subject> subject(new Subject());
                std::vector<Foo> foo_array(N);

                for (auto index : randomized)
					Foo::do_connect(*subject, foo_array[index]);
                timer.reset();
            }
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

    NOINLINE(static double connection(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N));
		Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            timer.reset();
            for (auto index : randomized)
				do_connect(subject, foo_array[index]);
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

    NOINLINE(static double emission(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N));
		Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
				do_connect(subject, foo_array[index]);
            timer.reset();
            emit(subject, rng);
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

    NOINLINE(static double combined(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N));
		Rng_t rng;
        std::size_t count = 1;

        timer.reset();

        for (; g_limit > timer.count<Timer_u>(); ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
				do_connect(subject, foo_array[index]);
            emit(subject, rng);
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }
};

template<class Subject, class Base>
chrono_timer SignalSlotBenchmark<Subject, Base>::timer;

} // namespace Benchmark;

#endif // BENCHMARK_HPP
