.PHONY: client
.PHONY: server
client:
	gcc client.c common.c -o client
server:
	gcc server.c common.c -o server
