TARGET := regxStr.out
SRC := $(wildcard *.cpp)

DEBUG := -g -O0 -DTEST -DNDEBUG
#RELEASE := -O2 -DNDEBUG

CXXFLAGS := -Wall $(DEBUG) $(RELEASE)
LIB := /usr/lib/libpcre.a

OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)
HEADER := $(wildcard *.h)

CXXFLAGS+=-MD

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIB)

clean :
	$(RM) *.o *.d $(TARGET)

love : clean all

lines :
	@echo $(HEADER) $(SRC) | xargs wc -l

.PHONY : all clean love lines

ifneq (${MAKECMDGOALS},clean)
ifneq (${MAKECMDGOALS},love)
ifneq (${MAKECMDGOALS},lines)
sinclude $(DEP)
endif
endif
endif
