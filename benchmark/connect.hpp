#ifndef BS1_BENCHMARK_HPP
#define BS1_BENCHMARK_HPP

#include "benchmark.hpp"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/signals.hpp>
#include <boost/signals2.hpp>
#include "lib/jl_signal/Signal.h"
#include "lib/jl_signal/StaticSignalConnectionAllocators.h"
#include "lib/eviltwin/observer.hpp"
#if USE_EvilTwin_Ext
#	include "lib/eviltwin/observer_fork.hpp"
#	include "lib/eviltwin/observer_safe.hpp"
#endif
#include "../nano_signal_slot.hpp"
#include "lib/signal11/Signal.h"
#if USE_YSLib
#	include "YSLib/Core/yevt.hpp"
#endif
#if USE_StdFunction
#	include <functional>
#endif

namespace Benchmark
{

template<typename PMF, typename C>
void connect(boost::signal<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	subject.connect(std::bind(pmf, &obj, std::placeholders::_1));
}

using Bs1 = SignalSlotBenchmark<boost::signal<void(minstd_rand&)>,
	boost::signals::trackable>;


template<typename PMF, typename C>
void
connect(boost::signals2::signal<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	subject.connect(std::bind(pmf, &obj, std::placeholders::_1));
}

using Bs2 = SignalSlotBenchmark<boost::signals2::signal<void(minstd_rand&)>,
	boost::signals2::trackable>;


template<typename PMF, typename C>
void connect(jl::Signal<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	subject.Connect(&obj, pmf);
}

using Jls
	= SignalSlotBenchmark<jl::Signal<void(minstd_rand&)>, jl::SignalObserver>;


template<typename PMF, typename C>
void
connect(obs::Subject<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	obj.reg.emplace_front(
		subject.registerObserver(std::bind(pmf, &obj, std::placeholders::_1)));
}

struct EvlReg
{
	forward_list<obs::detail::UniversalPtr> reg;
};

using Evl = SignalSlotBenchmark<obs::Subject<void(minstd_rand&)>, EvlReg>;

#if USE_EvilTwin_Ext
struct EvfReg
{
	forward_list<Nano::Fork::UniversalPtr> reg;
};

using Evf = SignalSlotBenchmark<obs::Subject<void(minstd_rand&)>, EvfReg>;


struct EvsReg
{
	forward_list<Nano::Safe::UniversalPtr> reg;
};

using Evs = SignalSlotBenchmark<obs::Subject<void(minstd_rand&)>, EvsReg>;
#endif


template<typename PMF, typename C>
void
connect(Nano::Signal<void(minstd_rand&)>& subject, PMF, C& obj)
{
	subject.connect<C, &C::handler>(obj);
}

using Nss = SignalSlotBenchmark<Nano::Signal<void(minstd_rand&)>,
	Nano::Observer>;


template<typename PMF, typename C>
void
connect(Signal11::Signal<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	obj.reg += subject.connect(&obj, pmf);
}

struct S11Reg
{
	Signal11::ConnectionScope reg;
};

using S11 = SignalSlotBenchmark<Signal11::Signal<void(minstd_rand&)>, S11Reg>;


#if USE_YSLib
template<typename PMF, typename C>
void
connect(YSLib::GEvent<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	subject += std::bind(pmf, &obj, std::placeholders::_1);
}

using Ys = SignalSlotBenchmark<YSLib::GEvent<void(minstd_rand&)>>;
#endif

#if USE_Pseudo
template<typename PMF, typename C>
void
connect(std::function<void(minstd_rand&)>& subject, PMF pmf, C& obj)
{
	subject = std::bind(pmf, &obj, std::placeholders::_1);
}

using Fn = SignalSlotBenchmark<std::function<void(minstd_rand&)>>;

template<typename PMF, typename C>
void
connect(std::pair<void*, void*>& subject, PMF, C& obj)
{
	subject = {&obj, nullptr};
}

using FnRaw = SignalSlotBenchmark<std::pair<void*, void*>>;

template<typename PMF, typename C>
void
connect(int&, PMF, C&)
{}

using FnNoOp = SignalSlotBenchmark<int>;
#endif

} // namespace Benchmark;

#endif // BS1_BENCHMARK_HPP
