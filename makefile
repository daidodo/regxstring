TARGET := regxStr.out
SRC := $(wildcard *.cpp)
CXXFLAGS := -Wall -g -O2
LIB :=

OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ.o=.d)
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
