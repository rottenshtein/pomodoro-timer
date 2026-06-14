CXX = g++
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags glib-2.0 libnotify)
LIBS = $(shell pkg-config --libs libnotify)
BUILDDIR = build
TARGET = $(BUILDDIR)/pomodoro_timer
SRCS = $(wildcard src/*.cpp)


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

run:
	$(MAKE) all;
	-./$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET)

