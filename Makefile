SRC_FOLDER			= "src"
EXECUTABLES			= "$(SRC_FOLDER)/clientserver"
BIN_FOLDER			= "bin"
DATABASE_DEFAULT	= "database"

all:
	cd $(SRC_FOLDER); make
	cd $(EXECUTABLES); make

install:
	if [ -d $(BIN_FOLDER) ]; then rm $(BIN_FOLDER) -r; fi
	mkdir $(BIN_FOLDER)
	
	mv $(EXECUTABLES)/client_main $(BIN_FOLDER)
	mv $(EXECUTABLES)/disk_server_main $(BIN_FOLDER)
	mv $(EXECUTABLES)/inmemory_server_main $(BIN_FOLDER)

clean:
	cd $(SRC_FOLDER); make clean
	cd $(EXECUTABLES); make clean
	if [ -d $(BIN_FOLDER) ]; then rm $(BIN_FOLDER) -r; fi
	if [ -d $(DATABASE_DEFAULT) ]; then rm $(DATABASE_DEFAULT) -r; fi

