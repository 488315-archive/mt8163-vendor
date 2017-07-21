#ifndef __FLEX_H__
#define __FLEX_H__

#if defined(WIN32) && !defined (__MINGW32__)
	#define WIN32_LEAN_AND_MEAN
	#define _inline_ _inline
#ifdef _MSC_VER
	#include "pstdint.h"
#endif
#else 
	#include <sys/types.h>
	#include <stdint.h>
	#include <ctype.h>
#endif


#ifdef __cplusplus
#define		EXTCDECL	extern "C"
#else
#define		EXTCDECL	extern
#endif

//
// All Server processing functions should have the same signature
typedef int (server_proc_function_t)(void * state);
typedef void *thread_start_t(void *);
EXTCDECL void create_working_thread(thread_start_t *p_thread_function, void *p_thread_data);

//
// **********   C wrappers for the flex_ class ******************
//
EXTCDECL void *init_connection(char *server_name, unsigned int port, unsigned int timeout_sec);
EXTCDECL void close_connection(void *state);
EXTCDECL int init_server(unsigned int port, server_proc_function_t *p_proc_func);
EXTCDECL void start_server();
EXTCDECL void close_server(void *state);
EXTCDECL int is_connected(void *state);

//
// Low level socket functions that send and receive raw data to/from specified buffers.
//

EXTCDECL int send_raw_data(void *state, const uint8_t * buffer, uint32_t write_size);
EXTCDECL int recv_raw_data(void *state, uint8_t * buffer, uint32_t read_size);

// High level functions to wait for and parse the received data
// If successful will return >=0, if error - will return -1
EXTCDECL int recv_cmd(void *state, uint8_t *result);
EXTCDECL int recv_param(void *state, uint32_t *set_flag, uint8_t *result, uint32_t num_bytes);
EXTCDECL int recv_int(void *state, int32_t *result);
EXTCDECL int recv_float(void *state, float *result);
EXTCDECL int recv_string(void *state, uint8_t *result, uint32_t max_chars);
EXTCDECL int recv_stream(void *state, uint8_t *result, uint32_t num_bytes);

// High level functions to send HEX ASCII data
// The data is buffered in the internal buffer and is sent on explicit call to buff_send()
EXTCDECL int buff_cmd(void *state, uint8_t data);
EXTCDECL int buff_int(void *state, uint32_t data);
EXTCDECL int buff_float(void *state, float data);
EXTCDECL int buff_string(void *state, uint8_t *buffer);
EXTCDECL int buff_stream(void *state, uint8_t *buffer, uint32_t num_bytes);
EXTCDECL int buff_bytes(void *state, uint8_t *buffer, uint32_t num_bytes);
EXTCDECL int buff_send(void *state);
//
// **********  End of C wrappers for the flex class *************
//

/*
    flex_ api description.

    1) All data passed between server and client are in ascii strings.
    2) A command and all its parameters is sent from the client as one long string.
    3) A response and all returned data is sent from the server as one long string.
    4) The client initiates everything, never the server. All commands get a response.
    5) All data sent is always hex, with no leading 0x. 
    6) All data is bytes, Bytes are two chars. If more than 2 - the previous assembled bytes are shifted left by 8
    7) All data is padded with 0s to the full width of the field.
    8) writelength is the length of data for the command, readlength is the data response.

    Command format:
    cmd_letter 32bitwritelength 32bitreadlength <register# or module#> data_bytes_needed_for_command

    example read i2c address 1 (cmd, write = 0, read = 1, reg = 0x54)
    F 0 1 54

    example get_id (cmd, write = 4, read = 4 ) module 0x12
    I 4 4 00 FF120000 

    example get_param(cmd, write = 4, read = 4, module=34 param = 0x12 )
    B 5 4 00 0334010012

    Response format: 
    cmd_letter 32bitstatus 32bitlength data_bytes_returned_by_command

    example read i2c address 0x54:
    R 0 1 c0

    example get_id response for rom id 00000042:
    I 0 4 00000042

    example get_param response 223344:
    G 0 1 00223344
	
	Errors are indicated by a negative status (FFFFFFFF) or -1
*/

typedef enum {
	// Highest level commands that have the knowledge of the Xtensa Middleware Protocol
	CP_GET_ID = 'A',			// Get module ID
	CP_GET_PARAM = 'B',			// Get Parameter(s) from Module
	CP_SET_PARAM = 'C',			// Set Parameter(s) to Module
	CP_GET_CONFIG = 'D',		// Get Configuration from Module
	CP_SET_CONFIG = 'E',		// Set Configuration to Module

	// Generic command to send n-bytes into the tunnel register (if the device has one), and to 
	//  receive m-bytes from that register.
	// This command may encapsulate ANY Middleware command to the Xtensa.
	CP_DATA = 'H',				// Download block of code to Xtensa and read back block of data
	CP_DATA_READ = 'I',			// Read back block of data from Xtensa core
	CP_DATA_WRITE = 'J',		// Write block of data to Xtensa core
	
	// Most generic commands that read and write individual registers on the chip.
	//  The types of registers are differentiated by the addressing, f.e.:
	//  0x0000 - 0x00FF - page 0
	//	0x01FF - 0x01FF - page 1
	//  0x0200 - 0x02FF - GPIO regs on EvKit board
	//	0x0300 - 0x03FF - SPI registers on EvKit board and so on...
	CP_REG_READ = 'F',			// Read multiple registers sequentially 
	CP_REG_WRITE = 'G',			// Write multiple registers sequentially


	// Lowest level device commands.
	// They perform device detection and Reset/taking out of Reset functionality
	CP_DEV_COUNT = 'K',			// Count attached devices and return the filled array of supported devices
	CP_DEV_OPEN = 'L',			// Check the connectivity and connect to the specified device
	CP_DEV_CLOSE = 'M',			// Close the connection to the specified device
	CP_DEV_TEST = 'T',			// Check (test) the connectivity 
	CP_DEV_INIT = 'S',			// Initialize the device and make it active/Take out of reset
	CP_DEV_DEINIT = 'R',		// De-init the device and make it non-active/Reset it

	CP_DEV_EXECUTE = 'X',		// Execute an arbitrary High-level (JS, or VBScript, or VB.NET, or J#) command
	CP_DEV_QUERY = 'Q',			// Query the Device if it wants to send us ANY arbitraty update
	
	// High level Parameter command
	CP_APP_PARAM = 'P'			// The high-level command that either reads or writes the parameter as a floating point
								// The format is as follows:
								//			"P BLK.MOD.PAR" to get the parameter in form of "NNNN.FFFF"
								//			"P BLK.MOD.PAR=NNNN.FFFF" to set it
}CommandType;

#endif