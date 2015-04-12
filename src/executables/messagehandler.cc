#include "messagehandler.h"
#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"

using namespace std;

MessageHandler::MessageHandler(const shared_ptr<Connection>& conn) : connection(conn) {}


void MessageHandler::sendByte(unsigned char code) {
	/* 	Connection::write() throws ConnectionClosedException. 
		Will be passed along to Client/Server and handled. */
	connection->write(code);
}

unsigned char MessageHandler::recvByte() {	
	/* 	Connection::read() throws ConnectionClosedException. 
		Will be passed along to Client/Server and handled. */
	unsigned char code = connection->read();
	return code;
}

void MessageHandler::send_code(unsigned char code) {
	sendByte(code);
}

unsigned char MessageHandler::recv_code() {
	unsigned char code = recvByte();
	return code;
}

void MessageHandler::sendInt(unsigned value) {
	sendByte((value >> 24) & 0xFF);
	sendByte((value >> 16) & 0xFF);
	sendByte((value >> 8)  & 0xFF);
	sendByte(value & 0xFF);
}
unsigned MessageHandler::recvInt() {
	unsigned char b1 = recvByte();
	unsigned char b2 = recvByte();
	unsigned char b3 = recvByte();
	unsigned char b4 = recvByte();
	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

void MessageHandler::send_int_p(unsigned param) {
	sendByte(Protocol::PAR_NUM);
	sendInt(param);
}

unsigned MessageHandler::recv_int_p() {
	unsigned char code = recv_code();
	if (code != Protocol::PAR_NUM) {
		/* do error stuff */
	}
	int value = recvInt();
	return value;
}

void MessageHandler::send_string_p(string s) {
	sendByte(Protocol::PAR_STRING);
	sendInt(s.size());
	for (char c : s) {
		connection->write(c);
	}	
}

string MessageHandler::recv_string_p() {
	unsigned char code = recv_code();
	if (code != Protocol::PAR_STRING) {
		/* do error stuff */
	}
	unsigned n = recvInt();
	
	string s("");
	for (unsigned i = 0; i != n; ++i) {
		char ch = connection->read();
		s+=ch;
	}
	return s;
}




