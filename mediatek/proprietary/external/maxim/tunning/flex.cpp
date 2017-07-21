/*
 * extensions to access a tcp server for the flex i/o
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <float.h>
#include "flex_data.h"

#define DEBUGF(...) printf( __VA_ARGS__)
#define RET_ERROR_IF(a)  if( a ) return -1

extern "C"
{

// Functions to start new thread and pass information to it
#if defined(WIN32) && !defined(__MINGW32__)
// The starting function of the new thread
void create_working_thread(thread_start_t *p_thread_function, void *p_thread_data)
{
	_beginthread((void (_cdecl *)(void *))p_thread_function, NULL, (void *) p_thread_data);
}

#else
void create_working_thread(thread_start_t *p_thread_function, void *p_thread_data)
{
	pthread_t threadId;
	pthread_attr_t threadAttr;

	// initialize the thread attribute
	pthread_attr_init(&threadAttr);
	// Set the stack size of the thread
	pthread_attr_setstacksize(&threadAttr, 1024*1024);
	// Set thread to detached state. No need for pthread_join
	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
	// Create the threads
	pthread_create(&threadId, NULL, (void *(*)(void *)) p_thread_function, p_thread_data);
	// Destroy the thread attributes
	pthread_attr_destroy(&threadAttr);
}
#endif

void *init_connection(char *server_name, unsigned int port, unsigned int timeout_sec)
{
	return flex::init_connection(server_name, port, timeout_sec);
}

void close_connection(void *state)
{
	flex::close(state);
}

int init_server(unsigned int port, server_proc_function_t *p_proc_func)
{
	return flex::init_server(port, p_proc_func);
}
void start_server()
{
	flex::start_server();
}

void close_server(void *state)
{
	flex::close(state);
}

int is_connected(void *state)
{
	return flex::IsConnected(state);
}

int send_raw_data(void *state, const uint8_t * buffer, uint32_t write_size)
{
	return flex::send(state, buffer, write_size);
}

int recv_raw_data(void *state, uint8_t * buffer, uint32_t read_size)
{
	return flex::recv(state, buffer, read_size);
}

// High level functions to wait for and parse the received data
// If successful will return >=0, if error - will return -1
int recv_cmd(void *state, uint8_t *result)
{
	return flex::p_this->recv_cmd(state, result);
}

int recv_param(void *state, uint32_t *set_flag, uint8_t *result, uint32_t num_bytes)
{
	return flex::p_this->recv_param(state, set_flag, result, num_bytes);
}

int recv_int(void *state, int32_t *result)
{
	return flex::p_this->recv_int(state, result);
}

int recv_float(void *state, float *result)
{
	return flex::p_this->recv_float(state, result);
}

int recv_string(void *state, uint8_t *result, uint32_t max_chars)
{
	return flex::p_this->recv_string(state, result, max_chars);
}

int recv_stream(void *state, uint8_t *result, uint32_t num_bytes)
{
	return flex::p_this->recv_stream(state, result, num_bytes);
}

// High level functions to send HEX ASCII data
// The data is buffered in the internal buffer and is sent on explicit call to buff_send()
int buff_cmd(void *state, uint8_t data)
{
	return flex::p_this->buff_cmd(state, data);
}
int buff_int(void *state, uint32_t data)
{
	return flex::p_this->buff_int(state, data);
}

int buff_float(void *state, float data)
{
	return flex::p_this->buff_float(state, data);
}

int buff_string(void *state, uint8_t *buffer)
{
	return flex::p_this->buff_string(state, buffer);
}

int buff_stream(void *state, uint8_t *buffer, uint32_t num_bytes)
{
	return flex::p_this->buff_stream(state, buffer, num_bytes);
}

int buff_bytes(void *state, uint8_t *buffer, uint32_t num_bytes)
{
	return flex::p_this->buff_bytes(state, buffer, num_bytes);
}

int buff_send(void *state)
{
	return flex::p_this->send(state);
}

}	// End of extern "C"

flex::flex()
{
	m_states = NULL;
	m_num_ports = 0;
	m_num_callbacks = 0;
	m_num_sockets = 0;

#ifdef WIN32
	{
		WSADATA wsaData;
		int retval;
		if ((retval = WSAStartup(0x202, &wsaData)) != 0)
		{
			DEBUGF("\nWSAStartup() failed with error %d ", retval);  print_error(retval);
			WSACleanup();
		}else
		{
			DEBUGF("\nWSAStartup() is OK.\n");
			SetStdOutToNewConsole();
		}
	}
#endif
}

flex::~flex()
{
	// Close and clean all connections
	while(m_states)
	{
		close(m_states);
	}
	for(int i = 0; i < m_num_sockets; i++)
	{
		closesocket(m_server_sockets[i]);
		m_server_sockets[i] = INVALID_SOCKET;
	}
	m_num_sockets = 0;
#ifdef WIN32
	{
		WSACleanup();
	}
#endif
}

void flex::close(void *state)
{
	flex_state_t	**pp_list;
	flex_state_t	*p_state = (flex_state_t *) state;

	if ( state && IsConnected(state) )
	{
		DEBUGF("\nClosing Socket ...\n");
        closesocket(p_state->m_conn_socket);
		p_state->m_conn_socket = INVALID_SOCKET;
    }

	// Remove the state from the list
	pp_list = &m_states;
	while(*pp_list && (*pp_list != p_state)) { pp_list = &((*pp_list)->p_next); }
	if(*pp_list && p_state){
		*pp_list = p_state->p_next;
		free(p_state);
		DEBUGF("\nclose(): state %p removed from the list\n", (void*) p_state);
	}
}

/*
 * This routine sets up Client connection to the server
 */
void *flex::init_connection(char *server_name, unsigned int port, unsigned int timeout_sec)
{
    int retval;
    unsigned int addr = 0;
    struct sockaddr_in server_ip;
    struct hostent *hp = NULL;
	SOCKET	conn_socket;
	flex_state_t	*p_state;
	flex_state_t	**pp_list;

    // Attempt to detect if we should call gethostbyname() or gethostbyaddr()
    if (inet_addr(server_name) == INADDR_NONE)
    {   // server address is a name
        hp = gethostbyname(server_name);
    }else
    { // Convert nnn.nnn address to a usable one
        addr = inet_addr(server_name);
        hp = gethostbyaddr((char *)&addr, 4, AF_INET);
    }

    // Copy the resolved information into the sockaddr_in structure
    memset(&server_ip, 0, sizeof(server_ip));
    if (!hp) { //lookup failed, use addr
        memcpy(&(server_ip.sin_addr), &addr, sizeof(addr));
    } else {
        server_ip.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    }

	// Open the socket
	server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(port);
    conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* Open a socket */
    if (conn_socket < 0 ) {
        DEBUGF("\nClient: Error Opening socket: Error %d ", _errno); print_error(_errno);
		return NULL;
    }
    DEBUGF("\nClient: socket() is OK.\n");

	// Make a socket an unblocking one
	u_long	nonblocking = 1;
	if( ioctlsocket(conn_socket, FIONBIO, &nonblocking))
	{
        DEBUGF("\nClient: ioctlsocket(nonblock) failed: %d ", _errno); print_error(_errno);
        closesocket(conn_socket);
		return NULL;
	}

	DEBUGF("\nClient: Client connecting to: %s.\n", (hp) ? hp->h_name : "unknown host");
    if (connect(conn_socket, (struct sockaddr*)&server_ip, sizeof(server_ip)) == SOCKET_ERROR)
    {
		int Error = _errno;
		if( (Error != WSAEWOULDBLOCK) && (Error !=  WSAEALREADY) && (Error != WSAEISCONN))
		{
			DEBUGF("\nClient: connect() failed: %d ", Error); print_error(Error);
	        closesocket(conn_socket);
			return NULL;
		}
    }
    DEBUGF("\nClient: connect() is OK - waiting for ACK\n");
	// Wait for connection with TIMEOUT Seconds timeout
	timeval connect_timeout;
	connect_timeout.tv_sec = timeout_sec;
	connect_timeout.tv_usec = 0;

	fd_set socket_status;
	FD_ZERO(&socket_status);
	FD_SET(conn_socket, &socket_status);
	retval = select(FD_SETSIZE, NULL, &socket_status, NULL, &connect_timeout);
	if(retval == 0)
	{
        DEBUGF("\nClient: Connect request timed out\n");
        closesocket(conn_socket);
		return NULL;
	}else if( retval == SOCKET_ERROR)
	{
        DEBUGF("\nClient: select() failed: %d ", _errno);  print_error(_errno);
        closesocket(conn_socket);
		return NULL;
	}
    DEBUGF("\nClient: select() is OK - connected\n");

	// Allocate, fill the state and add it to the list
	p_state = (flex_state_t	*)malloc(sizeof(flex_state_t));
	{
		p_state->m_conn_socket = conn_socket;
		p_state->m_timeout = timeout_sec;
		p_state->p_next = NULL;
		p_state->p_process = p_state->p_receive = p_state->RxBuffer;
		p_state->p_output =  p_state->p_sent = p_state->TxBuffer;
	}
	pp_list = &m_states;
	while(*pp_list) { pp_list = &((*pp_list)->p_next); }
	*pp_list = p_state;
	DEBUGF("\nClient: state %p added to the list\n", (void*) p_state);

	return p_state;
}

/*
 * This routine adds the callback functions to handle incoming connections from the clients.
 */
int flex::init_server(unsigned int port, server_proc_function_t *p_proc_func)
{
	// Add the specified callback to the list of registered callbacks
	RET_ERROR_IF(!p_proc_func);
	m_callbacks[m_num_callbacks].port = port;
	m_callbacks[m_num_callbacks].proc_func = p_proc_func;
	m_num_callbacks++;

	// If port is already allocated - do not add it
	for(int i = 0; i < m_num_ports; i++)
	{
		if(m_ports[i] == port) return 0;
	}
	m_ports[m_num_ports++] = port;
	return 0;
}


//
// This is a static function that is called by every thread created
//
void *flex::NewConnectionProc(  void *Parameter )
{
	flex_state_t		**pp_list;
	flex_state_t		*p_state;
	SOCKET				new_socket;
	struct				sockaddr_in sin;
	int					addrlen;
	int					local_port;
	int					curr_callback;
	int					ret;

	// get all info from the Parameter passed to a new thread and free memory
	flex_thread_t *conn_info = (flex_thread_t *) Parameter;
	new_socket = conn_info->new_socket;
	free(Parameter);

	DEBUGF("\nServer: New thread started\n");

	// Find the port on which the connection came
	addrlen = sizeof(sin);
	if(getsockname(new_socket, (struct sockaddr *)&sin, (socklen_t *) &addrlen) == 0 &&
			sin.sin_family == AF_INET &&  addrlen == sizeof(sin))
	{
		local_port = ntohs(sin.sin_port);
	}else
	{
		DEBUGF("\nServer: getsockname(): Error %d\n", _errno);
		return 0;
	}

	DEBUGF("\nServer: getsockname(): port %d\n", local_port);
	// Allocate the memory for the state, fill the state and add it to the list
	p_state = (flex_state_t	*)malloc(sizeof(flex_state_t));
	{
		p_state->m_conn_socket = new_socket;
		p_state->m_timeout = 24 * 60 * 60;		// set timeout to 1 day
		p_state->p_next = NULL;
		p_state->p_process = p_state->p_receive = p_state->RxBuffer;
		p_state->p_output =  p_state->p_sent = p_state->TxBuffer;
	}
	// Add to the list of active states/sockets
	pp_list = &flex::m_states;
	while(*pp_list) { pp_list = &((*pp_list)->p_next); }
	*pp_list = p_state;
	DEBUGF("\nServer: state %p  added to the list\n", (void*) p_state);

	// Go thru the list of all registered callbacks and give them a chance to
	//  do something useful.....
	for(curr_callback = 0; curr_callback < m_num_callbacks; curr_callback++)
	{
		if(m_callbacks[curr_callback].port == local_port)
		{
			// If function returns -1 - that means that it was its call and it wants
			//  to close the socket and exit the thread.
			// If return is 0 - then the command was not for that function, and we should pass it to the next
			// if return > 0 - then the module wants us to start from the beginning
			uint8_t	*p_saved = p_state->p_process;
			ret = m_callbacks[curr_callback].proc_func(p_state);
			if( ret < 0)
			{
				break;
			}else if( ret == 0)
			{
				// Reset processed pointer back to initial state for the next module to process
				// the data anew...
				p_state->p_process = p_saved;
			}else
			{
				// Start from the first module
				curr_callback = -1;
			}
		}
	}
	// Processing is done - close socket and release structure
	close(p_state);
	DEBUGF("\nServer: Thread terminated\n");
	return 0;
}

void flex::start_server()
{
    int request_from_len;
    struct sockaddr_in local_server, request_from;

	SOCKET	listen_socket, accepted_socket;

	fd_set socket_status;

	// Go thru the list of the registered server states and open sockets to listen
	for(int i = 0; i < m_num_ports; i++)
	{
		// Prepare the socket
		memset(&local_server, 0, sizeof(local_server));
		local_server.sin_addr.s_addr = INADDR_ANY;
		local_server.sin_family = AF_INET;
		local_server.sin_port = htons(m_ports[i]);

		// Open the socket
		listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* Open a socket */
		if (listen_socket < 0 ) {
			DEBUGF("\nServer: Error Opening socket: Error %d ", _errno);  print_error(_errno);
			continue;
		}
		DEBUGF("\nServer: socket() is OK.\n");

		// Make a socket an unblocking one
		u_long	nonblocking = 1;
		if( ioctlsocket(listen_socket, FIONBIO, &nonblocking))
		{
			DEBUGF("\nServer: ioctlsocket(nonblock) failed: %d ", _errno);  print_error(_errno);
			closesocket(listen_socket);
			continue;
		}

		// bind() associates a local address and port combination with the socket just created.
		if (bind(listen_socket, (struct sockaddr*)&local_server, sizeof(local_server)) == SOCKET_ERROR)
		{
			DEBUGF("\nServer: bind() failed: %d ", _errno);  print_error(_errno);
			closesocket(listen_socket);
			continue;
		}
		DEBUGF("\nServer: bind() is OK.\n");
		if (listen(listen_socket, 10) == SOCKET_ERROR)
		{
			DEBUGF("\nServer: listen() failed: %d ", _errno);  print_error(_errno);
			closesocket(listen_socket);
			continue;
		}
		DEBUGF("\nServer: listening on port: %d\n", m_ports[i]);
		m_server_sockets[m_num_sockets++] = listen_socket;
	}

	// Now wait for the connect request or any other data to be read
	if(m_num_sockets == 0)
	{
		DEBUGF("\nServer: no listening sockets are open - exiting....\n");
		return;
	}
	while(1)
	{
		int retval;
		// Fill out the structure with all sockets we are interested
		FD_ZERO(&socket_status);
		for(int i = 0; i < m_num_sockets; i++) FD_SET(m_server_sockets[i], &socket_status);
		// Check if there is any incoming connection request
		retval = select(FD_SETSIZE, &socket_status, NULL, NULL, NULL);
		if( retval == SOCKET_ERROR)
		{
			DEBUGF("\nServer: select() failed: %d ", _errno);  print_error(_errno);
			break;
		}
		DEBUGF("\nServer: select() is OK\n");
		for(int i = 0; i < m_num_sockets; i++)
		{
			if(FD_ISSET(m_server_sockets[i], &socket_status))
			{
				request_from_len = sizeof(request_from);
				accepted_socket = accept(m_server_sockets[i], (struct sockaddr *) &request_from, (socklen_t *)&request_from_len);
				if(accepted_socket < 0)
				{
					DEBUGF("\nServer: accept() failed: %d ", _errno);  print_error(_errno);
					break;
				}
				DEBUGF("\nServer: connection from %s, port %d\n", inet_ntoa(request_from.sin_addr),  htons(request_from.sin_port));

				// Spawn the new thread to handle the connection request
				// Create the information structure that we will pass to the thread and populate it
				flex_thread_t *conn_info = (flex_thread_t *)malloc(sizeof(flex_thread_t));
				conn_info->new_socket = accepted_socket;
				create_working_thread((thread_start_t *)&flex::NewConnectionProc, (void *) conn_info);
			}
		}
	}
}

/*
 * Write to server - low level, works with external buffers
 */
int flex::send(void *state, const uint8_t * buffer, uint32_t write_size)
{
    int retval;
	flex_state_t	*p_state = (flex_state_t	*) state;

    RET_ERROR_IF(!IsConnected(state));		// not connected

    while(write_size)
	{
		retval = ::send(p_state->m_conn_socket, (const char *)buffer, write_size, 0);
		if ((retval == SOCKET_ERROR) /* && (_errno != WSAEWOULDBLOCK) */) {
			DEBUGF("\nsend() failed: error %d ", _errno);  print_error(_errno);
			close(state);
			return -1;
		}
		buffer += retval;
		write_size -= retval;
	}
    return retval;
}

/*
 * Read from server - low level, works with external buffers
 */
int flex::recv(void *state, uint8_t * buffer, uint32_t read_size)
{
	fd_set socket_status;
	timeval connect_timeout;
    int retval;
	flex_state_t	*p_state = (flex_state_t *) state;

    RET_ERROR_IF(!IsConnected(state));		// not connected

	// Wait for connection with TIMEOUT Seconds timeout
	connect_timeout.tv_sec = p_state->m_timeout;
	connect_timeout.tv_usec = 0;

	// Prepare the structures for select() call
	FD_ZERO(&socket_status);
	FD_SET(p_state->m_conn_socket, &socket_status);

	retval = select(FD_SETSIZE, &socket_status, NULL, NULL, &connect_timeout);
	if(retval == 0)
	{
		DEBUGF("\nrecv() timed out\n");
		close(state);
		return -1;
	}else if( retval == SOCKET_ERROR)
	{
		DEBUGF("\nrcvd failed: %d ", _errno);  print_error(_errno);
		close(state);
		return -1;
	}

	retval = ::recv(p_state->m_conn_socket, (char *)buffer, read_size, 0);
    if (retval == 0) {
        DEBUGF("\nrecv(): Connection closed\n");
		close(state);
        return -1;
	}else if (retval == SOCKET_ERROR) {
        DEBUGF("\nrecv() failed: error %d ", _errno);  print_error(_errno);
		close(state);
        return -1;
    }
    return retval;
}

int flex::recv(void *state)
{
	flex_state_t	*p_state = (flex_state_t *) state;

	int		bytes_avail = sizeof(p_state->RxBuffer) - (uint32_t)(p_state->p_receive - p_state->RxBuffer);
	RET_ERROR_IF(bytes_avail <= 0);	// Error - return and report

	int		bytes_rcvd = recv(state, p_state->p_receive, bytes_avail);
	RET_ERROR_IF(bytes_rcvd <= 0);	// Error - return and report
	p_state->p_receive += bytes_rcvd;
	return (int)(p_state->p_receive - p_state->p_process);
}

bool flex::IsConnected(void *state)
{
	flex_state_t	*p_state = (flex_state_t *) state;
	return p_state && (p_state->m_conn_socket != INVALID_SOCKET);
}

//
// High level functions to receive block of data, longs, single characters
//   All of them ignore the leading spaces
//

int flex::recv_next_byte(void *state, uint8_t *result)
{
	flex_state_t	*p_state = (flex_state_t *) state;

	if(p_state->p_process >= p_state->p_receive)
	{
		p_state->p_process = p_state->p_receive = p_state->RxBuffer;
		RET_ERROR_IF( recv(state) <= 0 );	// Error - return and report
	}
	*result = *p_state->p_process++;
	return 1;
}

int flex::recv_cmd(void *state, uint8_t *result)
{
	uint8_t		Ch;
	flex_state_t	*p_state = (flex_state_t *) state;

	while(1)
	{
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report
		// Skip white spaces
		if(Ch && !isspace(Ch))
		{
			*result = Ch;
			break;
		}
	}
	return sizeof(uint8_t);
}

int flex::recv_param(void *state, uint32_t *set_flag, uint8_t *result, uint32_t num_bytes)
{
	PARSE_STATE	parse_state;
	uint8_t		Ch;
	flex_state_t	*p_state = (flex_state_t *) state;
	int		ret = 0;

	RET_ERROR_IF((result == 0) || (num_bytes <= 1));
	parse_state = SKIP_SPACES;
	num_bytes--;
	while(num_bytes)
	{
		// Receive more data and update the pointer. Check for errors.
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report

		if((parse_state == SKIP_SPACES) && !isspace(Ch))			// Skip white spaces
		{
			parse_state = COLLECT_PARAM;
		}
		if( parse_state == COLLECT_PARAM)
		{
			if( Ch && !isspace(Ch) && (Ch != '='))
			{
				result[ret++] = Ch;
				num_bytes--;
			}else
			{
				result[ret++] = '\0';
				if(set_flag)*set_flag = (Ch == '=') ? 1 : 0;
				break;
			}
		}
	}
	return ret;
}

int flex::recv_int(void *state, int32_t *result)
{
	PARSE_STATE	parse_state;
	uint8_t		Ch;
	int32_t		Data = 0;
	bool		NegFlag = false;
	flex_state_t	*p_state = (flex_state_t *) state;

	parse_state = SKIP_SPACES;

	while(1)
	{
		// Receive more data and update the pointer. Check for errors.
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report

		if((parse_state == SKIP_SPACES) && !isspace(Ch))			// Skip white spaces
		{
			parse_state = COLLECT_DIGIT;
		}
		if(parse_state == COLLECT_DIGIT)
		{
			if(Ch == '-')	NegFlag = true;
			else if( Ch >= '0' && Ch <= '9') Data = (Data << 4) + (Ch - '0');
			else if( Ch >= 'A' && Ch <= 'F') Data = (Data << 4) + (Ch - 'A' + 10);
			else if( Ch >= 'a' && Ch <= 'f') Data = (Data << 4) + (Ch - 'a' + 10);
			else
			{
				if(NegFlag)		Data = -Data;
				if(result)		*result = Data;
				break;
			}
		}
	}
	return sizeof(int32_t);
}

int flex::recv_float(void *state, float *result)
{
	PARSE_STATE	parse_state;
	uint8_t		Ch;
	float		Data = 0.0F;
	bool		NegFlag = false;
	float		Frac = 0.1F;
	flex_state_t	*p_state = (flex_state_t *) state;

	parse_state = SKIP_SPACES;

	while(1)
	{
		// Receive more data and update the pointer. Check for errors.
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report

		if((parse_state == SKIP_SPACES) && !isspace(Ch))			// Skip white spaces
		{
			parse_state = COLLECT_INT;
		}
		if(parse_state == COLLECT_INT)
		{
			if(Ch == '-')	NegFlag = true;
			else if( Ch >= '0' && Ch <= '9') Data = Data * 10.0F + (int)(Ch - '0');
			else if( Ch == '.' || Ch == ',')
			{
				parse_state = COLLECT_FRAC;
			}else
			{
				if(NegFlag)		Data = -Data;
				if(result)		*result = Data;
				break;
			}
		}else if(parse_state == COLLECT_FRAC)
		{
			if( Ch >= '0' && Ch <= '9')
			{
				Data = Data + (Ch - '0') * Frac;
				Frac = Frac * 0.1F;
			}else
			{
				if(NegFlag)		Data = -Data;
				if(result)		*result = Data;
				break;
			}
		}
	}
	return sizeof(float);
}

int flex::recv_string(void *state, uint8_t *result, uint32_t max_chars)
{
	PARSE_STATE	parse_state;
	uint8_t		Ch;
	uint32_t	nChars = 0;
	flex_state_t	*p_state = (flex_state_t *) state;

	parse_state = COLLECT_STRING;
	while(1)
	{
		// Receive more data and update the pointer. Check for errors.
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report

		if(parse_state == COLLECT_STRING)
		{
			if(Ch && max_chars && (Ch != '\n') && (Ch != '\r'))
			{
				*result++ = Ch;
				nChars++;
				max_chars--;
			}else
			{
				*result++ = '\0';
				break;
			}
		}
	}
	return nChars;
}


int flex::recv_stream(void *state, uint8_t *result, uint32_t num_bytes)
{
	PARSE_STATE	parse_state;
	uint8_t		Ch;
	uint8_t		Data = 0;
	uint32_t	NibbleCounter = num_bytes * 2;
	flex_state_t	*p_state = (flex_state_t *) state;

	parse_state = SKIP_SPACES;
	while(1)
	{
		// Receive more data and update the pointer. Check for errors.
		RET_ERROR_IF(recv_next_byte(state, &Ch) <= 0);	// Error - return and report

		if((parse_state == SKIP_SPACES) && (Ch && !isspace(Ch)))			// Skip white spaces
		{
			parse_state = COLLECT_STREAM;
		}
		if(parse_state == COLLECT_STREAM)
		{
			if( Ch >= '0' && Ch <= '9') {
				Data = (Data << 4) + (Ch - '0');
				NibbleCounter--;
			}else if( Ch >= 'A' && Ch <= 'F') {
				Data = (Data << 4) + (Ch - 'A' + 10);
				NibbleCounter--;
			}else if( Ch >= 'a' && Ch <= 'f') {
				Data = (Data << 4) + (Ch - 'a' + 10);
				NibbleCounter--;
			}else
				continue;
			if( (NibbleCounter & 0x0001) == 0x0000) {
				if(result) *result++ = Data;
				Data  = 0;
				if(NibbleCounter == 0) {
					break;
				}
			}
		}
	}
	return num_bytes;
}

//------------------------------------------------------------------------------------------------
// Static members definitions
//
uint8_t  flex::nibbleToHex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
flex    g_flex;
flex	*flex::p_this = &g_flex;

// Callbacks array and counter
flex_callback_t flex::m_callbacks[MAX_STATES];
int	flex::m_num_callbacks;

// Ports array and counter
int	flex::m_ports[MAX_STATES];
int flex::m_num_ports;

// Listening sockets array and counter
SOCKET	flex::m_server_sockets[MAX_STATES];
int flex::m_num_sockets;

// The linked list of the states
flex_state_t *flex::m_states;
//
//------------------------------------------------------------------------------------------------
//

int flex::buff_cmd(void *state, uint8_t cmd)
{
	flex_state_t	*p_state = (flex_state_t *) state;
	*p_state->p_output++ = cmd;
	*p_state->p_output++ = ' ';
	return 2;
}

int flex::buff_int(void *state, int32_t data)
{
	unsigned int i;
	int		ret = 0;
	flex_state_t	*p_state = (flex_state_t *) state;

	for(i = 0; (i < 8) && !(data & 0xF0000000) ; i++) data <<= 4;

	do{
		*p_state->p_output++ = flex::nibbleToHex[(data >> 28) & 0x0F];
		data <<= 4;
		ret++;
	}while(++i < 8);
	*p_state->p_output++ = ' ';	// Add space at the end
	ret++;
	return ret;
}

int flex::buff_float(void *state, float data)
{
	unsigned int i, i_temp;
	unsigned int int_part;					// Integer part of the number
	unsigned int const MAX_DIGITS = 16;		// Maximum number of digits (Integer + Fractional)
	uint8_t	 buff[MAX_DIGITS];
	int		ret = 0;
	flex_state_t	*p_state = (flex_state_t *) state;

	if(!isfinite(data))
	{
		data = -1.2345678f;
	}

	// Check and add negative sign (if necessary)
	if(data < 0)
	{
		data = -data;				// Make the number positive
		*p_state->p_output++ = '-';	// Output the sign
		ret++;
	}

	int_part = (unsigned int) data;
	data -= int_part;
	// Send first the integer part of the number
	for(i = 0; i < MAX_DIGITS; i++)
	{
		buff[i] = '0' + (int_part % 10);
		int_part = int_part / 10;
		if(0 == int_part) break;	// No more digits - exit
	}
	i_temp = i;
	RET_ERROR_IF( i >= MAX_DIGITS);

	// Move accumulated numbers from temp buffer to main in reverse
	do
	{
		*p_state->p_output++ = buff[i];
		ret++;
	}while(i--);

	// Add decimal point
	*p_state->p_output++ = '.';
	ret++;

	// Send the fractional part
	for( i = i_temp; (i < MAX_DIGITS); i++)
	{
		data *= 10;
		int_part = (unsigned int) data;
		*p_state->p_output++ = '0' + int_part;
		ret++;
		data -= int_part;
	}

	*p_state->p_output++ = ' ';	// Add space at the end
	ret++;
	return ret;
}

int flex::buff_string(void *state, uint8_t *buffer)
{
	uint32_t	num_bytes = 0;
	flex_state_t	*p_state = (flex_state_t *) state;
	if(buffer)
	{
		num_bytes = strlen((const char *)buffer);
		memcpy(p_state->p_output, buffer, num_bytes);
		p_state->p_output += num_bytes;
	}
	*p_state->p_output++ = '\0';
	return num_bytes;
}

int flex::buff_stream(void *state, uint8_t *buffer, uint32_t num_bytes)
{
	flex_state_t	*p_state = (flex_state_t *) state;
	int		ret = num_bytes * 2 + 1;  // Add a space at the end
	uint8_t		data;
	while(num_bytes--)
	{
		data = *buffer++;
		*p_state->p_output++ = flex::nibbleToHex[(data >> 4) & 0x0F];
		*p_state->p_output++ = flex::nibbleToHex[(data >> 0) & 0x0F];
	}
	*p_state->p_output++ = ' ';
	return ret;
}

int flex::buff_bytes(void *state, uint8_t *buffer, uint32_t num_bytes)
{
	flex_state_t	*p_state = (flex_state_t *) state;
	memcpy(p_state->p_output, buffer, num_bytes);
	p_state->p_output += num_bytes;
	return num_bytes;
}

int flex::send(void *state)
{
	int		num_bytes;
	int		bytes_sent;

	flex_state_t	*p_state = (flex_state_t *) state;
	num_bytes = (uint32_t)(p_state->p_output - p_state->p_sent);
	bytes_sent = send(state, p_state->p_sent, num_bytes);

	if(bytes_sent >= 0)
	{
		if(bytes_sent >= num_bytes)
		{
			// Reset the transmit buffer to the start
			p_state->p_output = p_state->p_sent = p_state->TxBuffer;
		}else
		{
			p_state->p_sent += bytes_sent;
		}
	}

	return bytes_sent;
}


/*
	Setup the DEBUGF console window for debugging purposes
*/
#ifdef WIN32
void flex::SetStdOutToNewConsole()
{
    // allocate a console for this app
    AllocConsole();

    // redirect unbuffered STDOUT to the console
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int fileDescriptor = _open_osfhandle((intptr_t)consoleHandle, _O_TEXT );
    FILE *fp = _fdopen( fileDescriptor, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // give the console window a nicer title
    SetConsoleTitle(_T("Debug Output with DEBUGF"));

    // give the console window a bigger buffer size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if ( GetConsoleScreenBufferInfo(consoleHandle, &csbi) )
    {
        COORD bufferSize;
        bufferSize.X = csbi.dwSize.X;
        bufferSize.Y = 9999;
        SetConsoleScreenBufferSize(consoleHandle, bufferSize);
    }
}
#endif
