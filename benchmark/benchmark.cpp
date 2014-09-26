#include "connect.hpp"
#include <iostream>
#include <iomanip>

using Table = map<string, vector<double>>;
using DataBase = map<string, Table>;
using namespace std;

template<class T>
void
test(const string& name, DataBase& records, size_t n)
{
	auto& rec(records[name]);
	T t;

	n = 1 << n;
	cout << n << ", " << name << endl;
	rec["construction"].emplace_back(t.construction(n));
	rec["destruction"].emplace_back(t.destruction(n));
	rec["connection"].emplace_back(t.connection(n));
	rec["emission"].emplace_back(t.emission(n));
	rec["combined"].emplace_back(t.combined(n));
}

int
main(int argc, char* argv[])
{
	using namespace Benchmark;
	constexpr size_t c_jlsignal_max = 9001;
	jl::StaticSignalConnectionAllocator<c_jlsignal_max> signal_con_allocator;
	jl::StaticObserverConnectionAllocator<c_jlsignal_max>
		observer_con_allocator;
	jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
	jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);
	DataBase records;

	for(size_t i(3); i <= 6; ++i)
	{
		test<Bs1>("Boost Signals", records, i);
		test<Bs2>("Boost Signals2", records, i);
		test<Evl>("EvilTwin Observer", records, i);
#if USE_EvilTwin_Ext
		test<Evf>("EvilTwin Observer Fork", records, i);
		test<Evs>("EvilTwin Observer Safe", records, i);
#endif
		test<Jls>("Jl_signal", records, i);
		test<Nss>("Nano-signal-slot", records, i);
		test<S11>("amc522 Signal11", records, i);
#if USE_YSLib
		test<Ys>("YSLib GEvent", records, i);
#endif
#if USE_Pseudo
		// Pseudo signal-slot.
		test<Fn>("function", records, i);
		test<FnRaw>("raw data copy", records, i);
		test<FnNoOp>("no-op", records, i);
#endif
	}
	for(const auto& table : records)
	{
		cout << "    " << table.first << ":";

		for(const auto& record : table.second)
		{
			const auto& data(record.second);

			cout << "\n ++ " << setw(40) << setfill('_')
				<< record.first << ": " << setprecision(2)
				<< fixed << accumulate(begin(data),
				end(data), 1.0) / double(data.size()) / 1000;
		}
		cout << endl;
	}
}
