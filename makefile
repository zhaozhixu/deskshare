CLIENT_DIR := ./client
SERVER_DIR := ./server

info:
	@echo "Available make targets:"
	@echo "	 debug  : make server and client binaries, debug version"
	@echo "	 release  : make server and client binaries, release version"
	@echo "  clean: clean all object files"

clean:
	rm -f $(CLIENT_DIR)/*.o
	rm -f $(SERVER_DIR)/*.o

debug:
	cd $(CLIENT_DIR) ; \
	make DEBUG=1
	cd $(SERVER_DIR) ; \
	make DEBUG=1

release:
	cd $(CLIENT_DIR) ; \
	make DEBUG=0
	cd $(SERVER_DIR) ; \
	make DEBUG=0
