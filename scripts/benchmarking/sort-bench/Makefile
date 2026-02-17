CXX=$(realpath .)/../aarch64/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-g++
CC=$(realpath .)/../aarch64/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc

ARCH := armv8-a

ENABLE_TIMER := 0
COMMONFLAGS := -O3 -g -march=$(ARCH) -DENABLE_TIMER=$(ENABLE_TIMER) -Wall -Wextra -Wno-unused-parameter -static
CXXFLAGS    := $(COMMONFLAGS) $(EFLAGS) -std=c++11
CFLAGS      := $(COMMONFLAGS) $(EFLAGS) -std=c99 -Wno-missing-field-initializers

HEADERS := $(wildcard *.h*)
SRCS    := $(wildcard *.cpp *.c)
OBJS    := $(SRCS:%.cpp=%.o)
OBJS    := $(OBJS:%.c=%.o)

# $(info "OBJS = $(OBJS)")

bench: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS)    $(CPPFLAGS) -c $<

clean:
	rm -f bench *.o
