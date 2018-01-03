CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)
CC = gcc
INC = $(shell pwd)
Repo = $(shell git rev-parse --show-toplevel)
SRC = $(Repo)/src

CPPFLAGS := $(shell root-config --cflags) -I$(INC)/include -I$(INC)/include/NetScope/ -I${HDF5_INCLUDE_PATH}
LDFLAGS := $(shell root-config --glibs) 
CPPFLAGS += -g -std=c++1y
#CPPFLAGS += -g 

LD_HDF5 = gcc
LIBS = -lm -L${HDF5_LIBRARY_PATH} -L/opt/local/lib -L/usr/local/lib -lpthread -lhdf5

TARGET = dat2root
SRC = dat2root.cc src/Aux.cc src/Config.cc

TARGET2 = Rereco
SRC2 = datroot2root.cc src/Aux.cc src/Config.cc

TARGET3 = dat2rootPixels
SRC3 = dat2rootPixels.cc src/Aux.cc src/Config.cc

TARGET4 = SkimTree
SRC4 = SkimTree.cc

OBJ = $(SRC:.cc=.o)
OBJ2 = $(SRC2:.cc=.o)
OBJ3 = $(SRC3:.cc=.o)
OBJ4 = $(SRC4:.cc=.o)
OBJ5 = $(SRC5:.cc=.o)

all : $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4)

NetScope: scopehdf2root convert_ots2hdf5

$(TARGET) : $(OBJ)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(TARGET2) : $(OBJ2)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET2) $(OBJ2) $(LDFLAGS)

$(TARGET3) : $(OBJ3)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET3) $(OBJ3) $(LDFLAGS)

$(TARGET4) : $(OBJ4)
	@echo $@
	$(LD) $(CPPFLAGS) -o $(TARGET4) $(OBJ4) $(LDFLAGS)

scopehdf2root: src/NetScope/scopehdf2root.C src/NetScope/hdf5io.o
	$(LD) $(CPPFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

convert_ots2hdf5: src/NetScope/convert_ots2hdf5.C src/NetScope/hdf5io.o src/NetScope/fifo.o
	$(LD) $(CPPFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

#src/NetScope/hdf5io.o: src/NetScope/hdf5io.cc 
#	$(CC) -Wall -O2 -DH5_NO_DEPRECATED_SYMBOLS -I$(INC)/include/NetScope/ -o $@ -c $<

%.o : %.cc
	@echo $@
	$(CXX) $(CPPFLAGS) -o $@ -c $<
clean :
	rm -rf *.o src/*.o $(Aux)/src/*.o src/NetScope/*.o $(TARGET) $(TARGET2) $(TARGET3) scopehdf2root convert_ots2hdf5 *.dSYM *~
