#ifndef NSS_BENCHMARK_HPP
#define NSS_BENCHMARK_HPP

#include "../nano_signal_slot.hpp"

#include "chrono_timer.hpp"
#include "benchmark.hpp"

#include <algorithm>
#include <memory>
#include <vector>

namespace Benchmark
{

class Nss : public Nano::Observer
{
    NOINLINE(void handler(Rng_t& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    using Subject = Nano::Signal<void(Rng_t&)>;
    using Foo = Benchmark::Nss;

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
            subject->connect<Foo, &Foo::handler>(foo_array.back());
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double destruction(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        std::shuffle(randomized.begin(), randomized.end(), rng);

        for (; elapsed < g_limit; ++count)
        {
            {
                std::unique_ptr<Subject> subject(new Subject);
                std::vector<Foo> foo_array(N);

                for (auto index : randomized)
                {
                    auto& foo = foo_array[index];
                    subject->connect<Foo, &Foo::handler>(foo);
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
        Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        std::shuffle(randomized.begin(), randomized.end(), rng);

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            timer.reset();
            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject.connect<Foo, &Foo::handler>(foo);
            }
            elapsed += timer.count<Timer_u>();
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }

//------------------------------------------------------------------------------

    NOINLINE(static double emission(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1, elapsed = 0;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        std::shuffle(randomized.begin(), randomized.end(), rng);

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject.connect<Foo, &Foo::handler>(foo);
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
        Rng_t rng;
        std::size_t count = 1;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        std::shuffle(randomized.begin(), randomized.end(), rng);

        timer.reset();

        for (; g_limit > timer.count<Timer_u>(); ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                subject.connect<Foo, &Foo::handler>(foo);
            }
            subject(rng);
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }
};

chrono_timer Nss::timer;

} // namespace Benchmark -------------------------------------------------------

#endif // NSS_BENCHMARK_HPP
