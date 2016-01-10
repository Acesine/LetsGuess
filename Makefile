SRC = src
BIN = bin
OUT = out

CLIENT_APP_NAME = client
SERVER_APP_NAME = server
SERVER_PORT_NUM = 5000

all: clean init build

init:
	mkdir -p $(SRC)
	mkdir -p $(BIN)
	mkdir -p $(OUT)
	cp words.txt $(BIN)/words.txt
build:
	g++ -c $(SRC)/lets_guess_server.cpp -o $(OUT)/lets_guess_server.o
	g++ -c $(SRC)/lets_guess_client.cpp -o $(OUT)/lets_guess_client.o
	g++ -c $(SRC)/utils.cpp -o $(OUT)/utils.o

bin: build
	g++ -o $(BIN)/$(SERVER_APP_NAME) $(OUT)/lets_guess_server.o $(OUT)/utils.o
	g++ -o $(BIN)/$(CLIENT_APP_NAME) $(OUT)/lets_guess_client.o $(OUT)/utils.o

clean:
	rm -f $(BIN)/*
	rm -f $(OUT)/*
