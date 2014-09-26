#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>
#include <memory>
#include <ytest/timing.hpp>
#include <YSLib/Adaptor/ycont.h>

#define NOINLINE __attribute__ ((noinline))

using std::map;
using std::vector;
using std::string;
using std::size_t;
using std::forward_list;
using namespace ytest::timing;

namespace Benchmark
{

using namespace std::chrono;
using std::minstd_rand;

template<typename F, typename... P>
static auto
emit(F&& f, P&&... args)
	-> decltype(static_cast<F&&>(f)(static_cast<P&&>(args)...))
{
	static_cast<F&&>(f)(static_cast<P&&>(args)...);
}
template<typename F, typename... P>
static auto
emit(F&& f, P&&... args)
	-> decltype(static_cast<F&&>(f).emit(static_cast<P&&>(args)...))
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


template<typename T, class R>
vector<T>
make_random_sequence(size_t n)
{
	R rng;
	vector<size_t> vec(n);

	std::iota(vec.begin(), vec.end(), size_t());
	std::shuffle(vec.begin(), vec.end(), rng);
	return std::move(vec);
}

template<typename _fCallable, typename _tDuration, typename Ret = double>
NOINLINE Ret
do_test(_fCallable f, _tDuration limit)
{
	return duration_cast<duration<Ret>>(average_in_duration(f, limit)).count();
}

template<typename _fCallable, typename _tDuration, typename Ret = double>
NOINLINE Ret
do_test_combined(_fCallable f, _tDuration limit)
{
	return duration_cast<duration<Ret>>(average_elapsed<_tDuration>(
		[&, limit, f](size_t& cnt, _tDuration& elapsed){
		for(auto now(high_resolution_clock::now()); (elapsed
			= duration_cast<_tDuration>(high_resolution_clock::now() - now))
			< limit; ++cnt)
			f();
	})).count();
}


struct NoTrack
{};


template<class Subject, class Base = NoTrack>
class SignalSlotBenchmark : public Base
{
public:
	using ThisType = Benchmark::SignalSlotBenchmark<Subject, Base>;

	nanoseconds Limit{milliseconds(8)};

	NOINLINE void
	handler(minstd_rand& rng)
	{
		volatile size_t r = rng();

		static_cast<void>(r);
	}

	static void
	do_connect(Subject& subject, ThisType& b)
	{
		connect(subject, &ThisType::handler, b);
	}

	double
	construction(size_t n)
	{
		return n / do_test([this, n]{
			std::unique_ptr<Subject> subject(new Subject());
			vector<ThisType> arr(n);

			return once(high_resolution_clock::now, [&]{
				do_connect(*subject, arr.back());
			});
		}, Limit);
	}

	double
	destruction(size_t n)
	{
		auto randomized(make_random_sequence<size_t, minstd_rand>(n));

		return n / do_test([&, this, n]{
			std::unique_ptr<Subject> subject(new Subject());
			vector<ThisType> arr(n);

			for(const auto index : randomized)
				ThisType::do_connect(*subject, arr[index]);
			return once(high_resolution_clock::now, [&]{
				subject.reset();
			});
		}, Limit);
	}

	double
	connection(size_t n)
	{
		auto randomized(make_random_sequence<size_t, minstd_rand>(n));

		return n / do_test([&, this, n]{
			Subject subject;
			vector<ThisType> arr(n);
			return once(high_resolution_clock::now, [&]{
				for(const auto index : randomized)
					do_connect(subject, arr[index]);
			});
		}, Limit);
	}

	double
	emission(size_t n)
	{
		auto randomized(make_random_sequence<size_t, minstd_rand>(n));
		minstd_rand rng;

		return n / do_test([&, this, n]{
			Subject subject;
			vector<ThisType> arr(n);

			for(auto index : randomized)
				do_connect(subject, arr[index]);
			return once(high_resolution_clock::now, [&]{
				emit(subject, rng);
			});
		}, Limit);
	}

	double
	combined(size_t n)
	{
		auto randomized(make_random_sequence<size_t, minstd_rand>(n));
		minstd_rand rng;

		return n / do_test_combined([&, this, n]{
			Subject subject;
			vector<ThisType> arr(n);

			for(const auto index : randomized)
				do_connect(subject, arr[index]);
			emit(subject, rng);
		}, Limit);
	}
};

} // namespace Benchmark;

#endif // BENCHMARK_HPP
