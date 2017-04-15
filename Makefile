CXXFLAGS += -std=c++11
OBJECTS = recdirscan.o main.o

all: $(OBJECTS) test

test: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o test $(OBJECTS)

clean:
	rm -f *.o
