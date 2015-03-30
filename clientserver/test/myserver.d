myserver.o myserver.d : myserver.cc ../server.h ../connection.h \
  ../connectionclosedexception.h ../protocol.h inmemory_database.h \
  database.h disk_database.h
