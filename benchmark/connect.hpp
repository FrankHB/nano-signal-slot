#ifndef BS1_BENCHMARK_HPP
#define BS1_BENCHMARK_HPP

#include "benchmark.hpp"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/signals.hpp>
#include <boost/signals2.hpp>
#include "lib/jl_signal/Signal.h"

namespace Benchmark
{

template<typename PMF, typename C>
void connect(boost::signal<void(Rng_t&)>& subject, PMF pmf, C& obj)
{
	subject.connect(std::bind(pmf, &obj, std::placeholders::_1));
}

using Bs1 = SignalSlotBenchmark<boost::signal<void(Rng_t&)>,
	boost::signals::trackable>;


template<typename PMF, typename C>
void connect(boost::signals2::signal<void(Rng_t&)>& subject, PMF pmf, C& obj)
{
	subject.connect(std::bind(pmf, &obj, std::placeholders::_1));
}

using Bs2 = SignalSlotBenchmark<boost::signals2::signal<void(Rng_t&)>,
	boost::signals2::trackable>;


template<typename PMF, typename C>
void connect(jl::Signal<void(Rng_t&)>& subject, PMF pmf, C& obj)
{
	subject.Connect(&obj, pmf);
}

using Jls = SignalSlotBenchmark<jl::Signal<void(Rng_t&)>,
	jl::SignalObserver>;

} // namespace Benchmark;

#endif // BS1_BENCHMARK_HPP
