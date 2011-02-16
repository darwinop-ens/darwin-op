CXX = g++

TARGET = roboplus

INCLUDE_DIRS = -I../../include -I../../../Framework/include
CXXFLAGS +=	-O2 -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
LIBS += -lpthread -lncurses -lrt

OBJS =	./main.o


all: darwin.a $(TARGET)

darwin.a:
	make -C ../../build

$(TARGET): $(OBJS) ../../lib/darwin.a
	$(CXX) -o $(TARGET) $(OBJS) ../../lib/darwin.a $(LIBS)
	
clean:
	rm -f $(OBJS) $(TARGET)





