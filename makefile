BIN_DIR := bin
OUT_TARGET := $(BIN_DIR)/regxstr
LIB_TARGET := $(BIN_DIR)/libregxstr.a
SO_TARGET :=  $(BIN_DIR)/libregxstr.so
INC_TARGET := regxstring.h

INSTALL_OUT_DIR := /usr/local/bin
INSTALL_LIB_DIR := /usr/lib
INSTALL_INC_DIR := /usr/include

DEBUG := -g -O0 -DTEST
#RELEASE := -O2 -DNDEBUG
CXXFLAGS := -Wall -fPIC $(DEBUG) $(RELEASE)
ARFLAGS := cr
LIB := /usr/lib/libpcre.a
CP := cp -f

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)
HEADER := $(wildcard *.h)

CXXFLAGS+=-MD

all : out lib so
	@$(CP) $(INC_TARGET) $(BIN_DIR)

install :
	$(CP) $(OUT_TARGET) $(INSTALL_OUT_DIR)
	$(CP) $(LIB_TARGET) $(SO_TARGET) $(INSTALL_LIB_DIR)
	$(CP) $(INC_TARGET) $(INSTALL_INC_DIR)

uninstall :
	$(RM) $(INSTALL_OUT_DIR)/$(notdir $(OUT_TARGET))
	$(RM) $(INSTALL_LIB_DIR)/$(notdir $(LIB_TARGET))
	$(RM) $(INSTALL_LIB_DIR)/$(notdir $(SO_TARGET))
	$(RM) $(INSTALL_INC_DIR)/$(INC_TARGET)

out : $(OUT_TARGET)

lib : $(LIB_TARGET)

so : $(SO_TARGET)

$(OUT_TARGET) : $(OBJ)
	$(CXX) -o $@ $^ $(LIB)

$(LIB_TARGET) : $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(SO_TARGET) : $(OBJ)
	$(CXX) -shared -fPIC -o $@ $^

cleandist : 
	$(RM) *.o

clean : cleandist
	$(RM) *.d $(BIN_DIR)/*

love : clean all

lines :
	@echo $(HEADER) $(SRC) | xargs wc -l

.PHONY : all install uninstall out lib so cleandist clean love lines

ifneq (${MAKECMDGOALS},install)
ifneq (${MAKECMDGOALS},uninstall)
ifneq (${MAKECMDGOALS},clean)
ifneq (${MAKECMDGOALS},cleandist)
ifneq (${MAKECMDGOALS},love)
ifneq (${MAKECMDGOALS},lines)
sinclude $(DEP)
endif
endif
endif
endif
endif
endif
