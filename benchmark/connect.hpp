#ifndef BS1_BENCHMARK_HPP
#define BS1_BENCHMARK_HPP

#include "benchmark.hpp"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/signals.hpp>
#include <boost/signals2.hpp>
#include "lib/jl_signal/Signal.h"
#include "lib/eviltwin/observer.hpp"
#include <forward_list>
#if USE_EvilTwin_Ext
#	include "lib/eviltwin/observer_fork.hpp"
#	include "lib/eviltwin/observer_safe.hpp"
#endif

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


template<typename PMF, typename C>
void connect(obs::Subject<void(Rng_t&)>& subject, PMF pmf, C& obj)
{
	obj.reg.emplace_front(
		subject.registerObserver(std::bind(pmf, &obj, std::placeholders::_1)));
}

struct EvlReg
{
	std::forward_list<obs::detail::UniversalPtr> reg;
};

using Evl = SignalSlotBenchmark<obs::Subject<void(Rng_t&)>, EvlReg>;

#if USE_EvilTwin_Ext
struct EvfReg
{
	std::forward_list<Nano::Fork::UniversalPtr> reg;
};

using Evf = SignalSlotBenchmark<obs::Subject<void(Rng_t&)>, EvfReg>;


struct EvsReg
{
	std::forward_list<Nano::Safe::UniversalPtr> reg;
};

using Evs = SignalSlotBenchmark<obs::Subject<void(Rng_t&)>, EvsReg>;
#endif

} // namespace Benchmark;

#endif // BS1_BENCHMARK_HPP
