

#ifndef INC_TCPCLIENT_H_
#define INC_TCPCLIENT_H_

#define  TCP_MSG_SIZE 64


void tcpclient_init (void);
void tcpsend (char *data);


typedef struct _TcpMessage
{
	char tcpMsg[TCP_MSG_SIZE];
}TcpMessage;

#endif /* INC_TCPCLIENT_H_ */
