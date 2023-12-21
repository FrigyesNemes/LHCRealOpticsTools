all: Match 

Match: Match.cc
	g++ --std=c++11 -Wall `root-config --libs` -lMinuit `root-config --cflags` \
		Match.cc -o Match
		
