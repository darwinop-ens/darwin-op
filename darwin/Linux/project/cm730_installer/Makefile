CXX = g++

TARGET = cm730_installer

INCLUDE_DIRS = -I../../include -I../../../Framework/include
CXXFLAGS +=	-O2 -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
LIBS += -lpthread -lncurses -lrt

OBJS =	./hex2bin.o \
		./main.o


all: $(TARGET)


$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	
clean:
	rm -f $(OBJS) $(TARGET)





