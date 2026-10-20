CXX=mpic++
CXXFLAGS=-Wall -O2 -I.
TARGET=mandelbrot

X11PATH=/usr/X11R6
LDFLAGS=-L$(X11PATH)/lib -lX11

OBJS=X11Display.o Palette.o mandelbrot.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)
clean:
	rm -f $(OBJS) $(TARGET)

