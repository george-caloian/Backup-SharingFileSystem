#
#  	Protocoale de comunicatii: 
#  	Laborator 6: UDP
#	mini-server de backup fisiere
# 	Makefile

CFLAGS = -Wall -g

# Portul pe care asculta serverul
PORT = 5000

# Adresa IP a serverului
IP_SERVER = 10.0.2.15

# Fisierul de unde citeste clientul
FILE_CLIENT = shared_files

# Fisierul unde salveaza serverul
FILE_SERVER = users_config

all: server client 

# Compileaza server.c
server: server.c

# Compileaza client.c
client: client.c

.PHONY: clean run_server run_client

# Ruleaza serverul
run_server:
	./server ${PORT} ${FILE_SERVER} ${FILE_CLIENT}

# Ruleaza clientul 	
run_client:
	./client ${IP_SERVER} ${PORT}

clean:
	rm -f server client
