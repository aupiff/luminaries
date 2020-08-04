default:
	g++ main.cpp -std=c++11 -o bin/lum

gif:
	convert -delay 0.1 -loop 0 $(ls -rt video/*) tl.gif
