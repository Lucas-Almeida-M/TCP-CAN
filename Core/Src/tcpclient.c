
#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "tcpclient.h"
#include "string.h"
static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t *addr, dest_addr;
static unsigned short port, dest_port;
extern osMessageQId queue_tcp_sendHandle;
bool Connected = false;
extern bool clientOnline;

TcpMessage tcpMessage = {0};
char msg[100];
char smsgc[200];
int indx = 0;


void tcpsend (char *data);


sys_sem_t tcpsem;


static void tcpinit_thread(void *arg)
{
	err_t err, connect_error;

	while (1)
	{
		conn = netconn_new(NETCONN_TCP);

		if (conn!=NULL)
		{

			err = netconn_bind(conn, IP_ADDR_ANY, 7);

			if (err == ERR_OK)
			{

				IP_ADDR4(&dest_addr, 192, 168, 0, 100);
				dest_port = 8000;

				connect_error = netconn_connect(conn, &dest_addr, dest_port);

				if (connect_error == ERR_OK)
				{
					Connected = true;
					sys_sem_signal(&tcpsem);
					while (1)
					{

						if (netconn_recv(conn, &buf) == ERR_OK)
						{

							addr = netbuf_fromaddr(buf);
							port = netbuf_fromport(buf);

							do
							{
								strncpy (msg, buf->p->payload, buf->p->len);
//								recv_tcp_msg(&msg);
								memset (msg, 0, 100);
							}
							while (netbuf_next(buf) > 0);

							netbuf_delete(buf);
						}
					}
				}

				else
				{
					netconn_close(conn);
					netconn_delete(conn);
				}
			}
			else
			{
				netconn_delete(conn);
			}
		}
		osDelay(1000);
	}
}

void tcpsend (char *data)
{
	if (clientOnline)
	{
		netconn_write(conn, data, strlen(data), NETCONN_COPY);

		sys_sem_signal(&tcpsem);
	}

}


void SendTCP_MSG(void const * argument)
{
  /* USER CODE BEGIN SendTCP_MSG */
	TcpMessage tcpMessage;
  /* Infinite loop */
  for(;;)
  {

	BaseType_t xStatus = xQueueReceive(queue_tcp_sendHandle, &tcpMessage.tcpMsg, 0);
	if (xStatus == pdPASS)
	{
		tcpsend(tcpMessage.tcpMsg);
	}
    osDelay(1);
  }
  /* USER CODE END SendTCP_MSG */
}




void tcpclient_init (void)
{
	sys_sem_new(&tcpsem, 0);
	sys_thread_new("tcpinit_thread", tcpinit_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}
