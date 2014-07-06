// g++ -O3 -s -std=c++11 benchmark.cpp lib/jl_signal/*.cpp
//	-llibboost_signals-mt -I/usr/include
//	-DUSE_EvilTwin_Ext=1 -DUSE_YSLib=1 -DUSE_Pseudo=1
#include "benchmark.hpp"

#include "connect.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <vector>
#include <map>
#include <string>

using Table = std::map<std::string, std::vector<double>>;
using DataBase = std::map<std::string, Table>;

//std::size_t g_limit = Timer_u(Limit_u(4)).count();
std::size_t g_limit = 40000000;

//------------------------------------------------------------------------------

void outputReport(DataBase const& records, std::ostream& ost)
{
    for (auto const& table : records)
    {
        auto tableName = table.first;

        ost << "\n    " << tableName << ":";

        for (auto const& record : table.second)
        {
            auto recordName = record.first;
            auto data = record.second;

            ost << "\n ++ " << std::setw(40) << std::setfill('_')
                << recordName << ": " << std::setprecision(2) << std::fixed
                << std::accumulate(std::begin(data), std::end(data), 1.0)
                / (double) data.size();
        }
        ost << std::endl;
    }
}

//------------------------------------------------------------------------------

template<class T>
int
test(const std::string& name, DataBase& records, std::size_t n)
{
    auto& rec = records[name];

	n = 1 << n;
	std::cout << n << ", " << name << std::endl;
    rec["construction"].emplace_back(T::construction(n));
    rec["destruction"].emplace_back(T::destruction(n));
    rec["connection"].emplace_back(T::connection(n));
    rec["emission"].emplace_back(T::emission(n));
    rec["combined"].emplace_back(T::combined(n));
}

int main(int argc, char* argv[])
{
    using namespace Benchmark;
    jl::StaticSignalConnectionAllocator<c_jlsignal_max> signal_con_allocator;
    jl::StaticObserverConnectionAllocator<c_jlsignal_max> observer_con_allocator;
    jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
    jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);
    DataBase records;

    for (std::size_t i(3); i <= 6; ++i)
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
		test<Fn>("std::function", records, i);
		test<FnRaw>("raw data copy", records, i);
		test<FnNoOp>("no-op", records, i);
#endif
    }
    if (std::ofstream ofs{"report.txt", std::ios::app})
    {
        outputReport(records, ofs);
        outputReport(records, std::cout);
        ofs << std::endl;
    }
    else
    {
        std::cerr << "ofstream error" << std::endl;
    }
}
