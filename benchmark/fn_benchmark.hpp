#ifndef Fn_BENCHMARK_HPP
#define Fn_BENCHMARK_HPP

#include "benchmark.hpp"

#include <functional>

namespace Benchmark
{

class Fn : public Nano::Observer
{
    NOINLINE(void handler(Rng_t& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    using Subject = std::function<void(Rng_t&)>;
    using Foo = Benchmark::Fn;

    static chrono_timer timer;

    public:

//------------------------------------------------------------------------------

    NOINLINE(static double construction(std::size_t N))
    {
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            timer.reset();
            std::unique_ptr<Subject> subject(new Subject);
            std::vector<Foo> foo_array(N);
        //	subject->connect<Foo, &Foo::handler>(foo_array.back());
            (*subject) = std::bind(&Foo::handler, &foo_array.back(),
				std::placeholders::_1);
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double destruction(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N)); Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            {
                std::unique_ptr<Subject> subject(new Subject);
                std::vector<Foo> foo_array(N);

                for (auto index : randomized)
                {
                    auto& foo = foo_array[index];
                    (*subject) = std::bind(&Foo::handler, &foo,
						std::placeholders::_1);
                }
                timer.reset();
            }
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double connection(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N)); Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            timer.reset();
            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject = std::bind(&Foo::handler, &foo,
					std::placeholders::_1);
            }
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double emission(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N)); Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject = std::bind(&Foo::handler, &foo,
					std::placeholders::_1);
            }
            timer.reset();
            subject(rng);
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double combined(std::size_t N))
    {
        auto randomized(make_random_sequence<std::size_t, Rng_t>(N)); Rng_t rng;
        std::size_t count = 1;

        timer.reset();

        for (; g_limit > timer.count<Timer_u>(); ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject = std::bind(&Foo::handler, &foo,
					std::placeholders::_1);
            }
            subject(rng);
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }
};

chrono_timer Fn::timer;

} // namespace Benchmark -------------------------------------------------------

#endif // Fn_BENCHMARK_HPP
