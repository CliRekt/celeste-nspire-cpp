EXE = celeste
OBJS = main.o

CXX = nspire-g++
LD = nspire-ld
GENZEHN = genzehn
PYTHON = python3

CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = -lndls

all: $(EXE).tns

assets.h: convert_assets.py
	$(PYTHON) convert_assets.py

main.o: main.cpp assets.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

$(EXE).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $< --output $@ --name "$(EXE)"

clean:
	rm -f $(OBJS) $(EXE).elf $(EXE).tns assets.h
