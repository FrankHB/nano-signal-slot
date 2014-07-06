#ifndef EVS_BENCHMARK_HPP
#define EVS_BENCHMARK_HPP

#include "benchmark.hpp"
#include <forward_list>

#include "lib/eviltwin/observer_safe.hpp"

namespace Benchmark
{

class Evs
{
    std::forward_list<Nano::Safe::UniversalPtr> reg;

    NOINLINE(void handler(Rng_t& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    using Subject = Nano::Safe::Subject<void(Rng_t&)>;
    using Foo = Benchmark::Evs;

    static chrono_timer timer;

    public:

//------------------------------------------------------------------------------

    NOINLINE(static double construction(std::size_t N))
    {
        std::size_t count = 1, elapsed = 0;

        using std::placeholders::_1;

        for (; elapsed < g_limit; ++count)
        {
            timer.reset();
            std::unique_ptr<Subject> subject(new Subject);
            std::vector<Foo> foo_array(N);
            auto& foo = foo_array.back();
            foo.reg.emplace_front(subject->connect
                (std::bind(&Foo::handler, &foo, _1)));
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

        using std::placeholders::_1;

        for (; elapsed < g_limit; ++count)
        {
            {
                std::unique_ptr<Subject> subject(new Subject);
                std::vector<Foo> foo_array(N);

                for (auto index : randomized)
                {
                    auto& foo = foo_array[index];
                    foo.reg.emplace_front(subject->connect
                        (std::bind(&Foo::handler, &foo, _1)));
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
        
        using std::placeholders::_1;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            timer.reset();
            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.connect
                    (std::bind(&Foo::handler, &foo, _1)));
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
        
        using std::placeholders::_1;

        for (; elapsed < g_limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.connect
                    (std::bind(&Foo::handler, &foo, _1)));
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

        using std::placeholders::_1;

        timer.reset();

        for (; g_limit > timer.count<Timer_u>(); ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.connect
                    (std::bind(&Foo::handler, &foo, _1)));
            }
            subject(rng);
        }
        return N / std::chrono::duration_cast<Delta_u>
            (Timer_u(g_limit / count)).count();
    }
};

chrono_timer Evs::timer;

} // namespace Benchmark -------------------------------------------------------

#endif // EVS_BENCHMARK_HPP
