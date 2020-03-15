OPTS = -Wall -O3 -std=c++0x
LIBS = `pkg-config opencv --cflags --libs`

calibration: calibration.cpp
	g++ ${OPTS} calibration.cpp -o calibration ${LIBS}

calibrationCircles: calibrationCircles.cpp
	g++ ${OPTS} calibrationCircles.cpp -o calibrationCircles ${LIBS}

calibrationACircles: calibrationACircles.cpp
	g++ ${OPTS} calibrationACircles.cpp -o calibrationACircles ${LIBS}

projection: projection.cpp
	g++ ${OPTS} projection.cpp -o projection ${LIBS}

clean:
	rm calibration
