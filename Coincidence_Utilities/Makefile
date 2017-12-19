CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)
INC = $(shell pwd)
Repo = $(shell git rev-parse --show-toplevel)
SRC = $(Repo)/src

CPPFLAGS := $(shell root-config --cflags) -I$(INC)/include 
LDFLAGS := $(shell root-config --glibs) 
CPPFLAGS += -g -std=c++1y

TARGET = ConvertTOFPETSinglesToEvents
SRC = ConvertTOFPETSinglesToEvents.cc
OBJ = $(SRC:.cc=.o)

TARGET2 = MergeTOFPETWithPixels
SRC2 = MergeTOFPETWithPixels.cc
OBJ2 = $(SRC2:.cc=.o)


all : $(TARGET) $(TARGET2)

$(TARGET) : $(OBJ)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(TARGET2) : $(OBJ2)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET2) $(OBJ2) $(LDFLAGS)

%.o : %.cc
	@echo $@
	$(CXX) $(CPPFLAGS) -o $@ -c $<
clean :
	rm -f *.o $(TARGET) *~
