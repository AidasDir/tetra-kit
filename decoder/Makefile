CC = g++
CFLAGS = -O2 -fPIC -std=c++11 -Wall -ljson-c -lz

SRC = decoder_main.cc coding.cc report.cc utils.cc viterbi.cc tetra_dl.cc mac.cc llc.cc mle.cc cmce.cc uplane.cc base64.cc
OBJ = $(SRC:.cc=.o)
EXE = decoder

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

test: test.o
	$(CC) $(CFLAGS) test.o -o $@

clean:
	rm -f $(OBJ) $(EXE) *.o *~
