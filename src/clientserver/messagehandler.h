#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "connection.h"
#include <stdexcept>
#include <string>
#include <memory>



class MessageHandler {

public:
	MessageHandler(const std::shared_ptr<Connection>&);
	void send_code(unsigned char);
	unsigned char recv_code();
	void send_int_p(unsigned);
	void send_string_p(std::string);
	unsigned recv_int_p();
	std::string recv_string_p();
	
private:
	std::shared_ptr<Connection> connection;
	
	void sendByte(unsigned char);	
	unsigned char recvByte();
	void sendInt(unsigned);		
	unsigned recvInt();
	

};



#endif
