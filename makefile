CXX 	   = g++
CXXFLAGS   = -std=c++17
OPENCV     = `pkg-config --cflags --libs opencv`
EXECUTABLE = MedianFilter

all: $(EXECUTABLE) clean

$(EXECUTABLE): main.o  MedianFilterCV_8U.o
	$(CXX) $(CXXFLAGS) $(OPENCV) main.o MedianFilterCV_8U.o -o $(EXECUTABLE)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

MedianFilterCV_8U.o: MedianFilterCV_8U.cpp    
	$(CXX) $(CXXFLAGS) -c MedianFilterCV_8U.cpp

clean:
	rm -rf *.o $(EXECUTABLE)
