.PHONY: client
.PHONY: server
client:
	gcc client.c common.c -o build/client
server:
	gcc server.c common.c -o build/server
