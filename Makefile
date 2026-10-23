EXE = celeste
OBJS = main.o assets.o

CXX = nspire-g++
LD = nspire-ld
GENZEHN = genzehn
PYTHON = python3

CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = -lndls

all: $(EXE).tns

assets.h assets.cpp: convert_assets.py convert_p8.py celeste.p8
	$(PYTHON) convert_assets.py
	$(PYTHON) convert_p8.py

assets.o: assets.cpp assets.h
	$(CXX) $(CXXFLAGS) -c assets.cpp -o assets.o

main.o: main.cpp assets.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

$(EXE).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $< --output $@ --name "$(EXE)"

clean:
	rm -f $(OBJS) $(EXE).elf $(EXE).tns assets.h assets.cpp
