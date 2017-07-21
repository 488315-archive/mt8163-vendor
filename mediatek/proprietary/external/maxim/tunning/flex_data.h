#ifndef __FLEX_DATA_H__
#define __FLEX_DATA_H__

#include	"flex.h"

#if defined(WIN32) && !defined(__MINGW32__)
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <WS2tcpip.h>
	#include <process.h>
#ifdef _MSC_VER
	#include "pstdint.h"
	#define isfinite(a) _finite(a)
#endif
	#include <io.h>
	#include <TChar.h>

	#define _inline_ _inline
	#define _errno WSAGetLastError()
	#define print_error(a) {LPTSTR s = NULL; \
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
               NULL, (a), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&s, 0, NULL); \
			   printf("%s\n", s); LocalFree(s);}while(0)
#else 
	#include <pthread.h>
#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <WS2tcpip.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#define WSAEWOULDBLOCK EWOULDBLOCK
	#define WSAEALREADY EALREADY
	#define WSAEISCONN EISCONN
	#define SOCKET int
	#define SOCKET_ERROR (-1)
	#define INVALID_SOCKET (-1)
	#define ioctlsocket ioctl
	#define closesocket(a) {::shutdown((a), 2); ::close(a);} while(0)
#endif	
	#include <stdint.h>
	#include <ctype.h>
	#include <unistd.h>
	#define _errno errno
	#define _T(a) a
	#define print_error(a){char *s = strerror(a); printf("%s\n", s);}while(0)
#endif

//
//		Defines that specify the max number of connected clients/servers
//			and the max buffer size for Rx and Tx
//
#define	MAX_STATES	(64)
#define	BUFFER_SIZE	(1024)

typedef enum {
	SKIP_SPACES = 0,
	COLLECT_CHAR = 1,
	COLLECT_DIGIT = 2,
	COLLECT_STREAM = 3,
	COLLECT_INT = 4,
	COLLECT_FRAC = 5,
	COLLECT_PARAM = 6,
	COLLECT_STRING = 7
} PARSE_STATE;

typedef struct flex_thread_t {
	SOCKET	new_socket;
} flex_thread_t;


typedef struct flex_state_t
{
	SOCKET			m_conn_socket;		// Valid if >=0, not connected if < 0
	unsigned int	m_timeout;			// Timeout for connection (Client only)

	// Private members to do the processing of received characters
	uint8_t			RxBuffer[BUFFER_SIZE];	
	uint8_t			TxBuffer[BUFFER_SIZE];	
	//  Rx pointers - if at some point (p_receive == p_process), then we have to reset them to RxBuffer
	uint8_t			*p_receive;		// Pointer to the next available location to receive data
	uint8_t			*p_process;		// Pointer to the next location that will be processed (p_process <= p_receive)
	// Tx pointers  - if at some point (p_output == p_send), then reset both to TxBuffer
	uint8_t			*p_output;		// Tracks the next available position to place the data to be sent
	uint8_t			*p_sent;		// Pointer to the last sent data

	// Link to the next structure element in the linked list
	flex_state_t *p_next;
} flex_state_t;

typedef struct flex_callback_t {
	int port;
	server_proc_function_t *proc_func;
}flex_callback_t;

#ifdef __cplusplus
	class flex 
	{
	public:
		flex();
		~flex(); 

		static void *init_connection(char *server_name, unsigned int port, unsigned int timeout_sec = 5);
		static int init_server(unsigned int port, server_proc_function_t *p_proc_func);
		static void start_server();


		// Low level functions that accept external buffers
		static int send(void *state, const uint8_t * buffer, uint32_t write_size);
		static int recv(void *state, uint8_t * buffer, uint32_t read_size);
		static void close(void *state);

		static bool IsConnected(void *state); 
	
		// High level functions to wait for and parse the received data
		// If successful will return >=0, if error - will return -1
		int recv_cmd(void *state, uint8_t *result);
		int recv_int(void *state, int32_t *result);
		int recv_float(void *state, float *result);
		int recv_string(void *state, uint8_t *result, uint32_t max_chars);
		int recv_stream(void *state, uint8_t *result, uint32_t num_bytes);
		int recv_param(void *state, uint32_t *set_flag, uint8_t *result, uint32_t num_bytes);

		// High level functions to send HEX ASCII data
		// The data is buffered in the internal buffer and is sent on explicit call to buff_send()
		int buff_cmd(void *state, uint8_t cmd);
		int buff_int(void *state, int32_t data);
		int buff_float(void *state, float data);
		int buff_string(void *state, uint8_t *buffer);
		int buff_stream(void *state, uint8_t *buffer, uint32_t num_bytes);
		int buff_bytes(void *state, uint8_t *buffer, uint32_t num_bytes);
		int send(void *state);

		static flex	*p_this;

	private:
		static flex_callback_t m_callbacks[MAX_STATES];
		static int	m_num_callbacks;
		
		static int m_ports[MAX_STATES];
		static int m_num_ports;

		static SOCKET	m_server_sockets[MAX_STATES];
		static int		m_num_sockets;

		static flex_state_t *m_states;
		
		static uint8_t  nibbleToHex[16];

		int recv(void *state);
		int recv_next_byte(void *state, uint8_t *result);

		// The function that is called by every new connecting thread
		static thread_start_t NewConnectionProc;
	#ifdef WIN32
		void SetStdOutToNewConsole();
	#endif
	};
#endif		// __cplusplus

#endif	// __FLEX_DATA_H__
