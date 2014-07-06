#include "benchmark.hpp"

#include "bs1_benchmark.hpp"
#include "bs2_benchmark.hpp"
#include "evl_benchmark.hpp"
#if USE_EvilTwin_Ext
#	include "evf_benchmark.hpp"
#	include "evs_benchmark.hpp"
#endif
#include "jls_benchmark.hpp"
#include "nss_benchmark.hpp"
#include "s11_benchmark.hpp"
#if USE_YSLib
#	include "ys_benchmark.hpp"
#endif
#if USE_StdFunction
#	include "fn_benchmark.hpp"
#endif

#include "lib/jl_signal/Signal.h"
#include "lib/jl_signal/StaticSignalConnectionAllocators.h"

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
#if USE_StdFunction
		// Pseudo signal-slot.
		test<Fn>("std::function", records, i);
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
