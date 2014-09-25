g++ -O3 -s -std=c++11 benchmark.cpp lib/jl_signal/*.cpp -llibboost_signals-mt \
	-I/usr/include -DUSE_EvilTwin_Ext=1 -DUSE_YSLib=1 -DUSE_Pseudo=1

