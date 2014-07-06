#ifndef BS1_BENCHMARK_HPP
#define BS1_BENCHMARK_HPP

#include "benchmark.hpp"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/signals.hpp>
#include <boost/signals2.hpp>

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

} // namespace Benchmark;

#endif // BS1_BENCHMARK_HPP
