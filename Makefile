CC=g++
CFLAGS=-c -std=c++1y -O0 -Wall -pedantic -g -lboost_system -lboost_filesystem -fPIC -pthread
LDFLAGS= -g -O0 -lboost_system -lboost_filesystem -ldl -rdynamic
SOURCES=Battery.cpp House.cpp Position.cpp Sensor.cpp SimpleIniFileParser.cpp SimulatedHouse.cpp SimulatedRobot.cpp Simulator.cpp Validator.cpp HouseMap.cpp DefaultScoreFormula.cpp AlgorithmRegistration.cpp AlgorithmRegistrar.cpp Encoder.cpp Montage.cpp

DYN_SRC=301645974_A_.cpp 301645974_B_.cpp 301645974_C_.cpp
DYN_OBJ=$(DYN_SRC:.cpp=.o)
DYN_SO=$(DYN_SRC:.cpp=.so)

DEPENDS= Battery.o Position.o HouseMap.o

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=simulator

all: $(SOURCES) $(EXECUTABLE) so
    
$(EXECUTABLE): $(OBJECTS) 

	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

so: $(DYN_SO)

$(DYN_SO): $(DYN_OBJ)
	$(CC) -g -O0 -shared -Wl,-soname,$@ -o $@ $(DEPENDS) $(@:.so=.o)

$(DYN_OBJ): $(DYN_SRC)
	g++ -g -O0 -c -std=c++1y $(@:.o=.cpp) -mcmodel=large -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)
	rm $(DYN_SO) 
	rm $(DYN_OBJ)	
