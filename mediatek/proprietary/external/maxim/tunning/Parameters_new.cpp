#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <flex.h>
#include <utils/String8.h>
#include <media/AudioSystem.h>
#include <media/AudioParameter.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define error(...) do {\
	fprintf(stderr, "%s: %s:%d: ", command, __FUNCTION__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	putc('\n', stderr); \
} while (0)
#else
#define error(args...) do {\
	fprintf(stderr, "%s: %s:%d: ", command, __FUNCTION__, __LINE__); \
	fprintf(stderr, ##args); \
	putc('\n', stderr); \
} while (0)
#endif

namespace android {
	typedef enum PARAM_TYPE { PARAM_NONE, PARAM_INT, PARAM_FLT, PARAM_DBL, PARAM_STR, PARAM_EXE_INT, PARAM_EXE_FLT, PARAM_EXE_DBL, PARAM_EXE_STR } PARAM_TYPE;

	typedef struct PARAM_TABLE{
		char		*pName;		// The name of the parameter
		PARAM_TYPE      nType;      // Parameter type - int, float, double, string, function
		int             id;             //command id
	}PARAM_TABLE;

	static PARAM_TABLE ParamTable[] = 
	{
		{"DSM.PUB.GAIN", PARAM_FLT, 5 }, 
		{"DSM.PUB.ENABLE", PARAM_FLT, 1 }, 
		{"DSM.PUB.EXC", PARAM_FLT, 30 }, 
		{"DSM.PUB.RDC", PARAM_FLT, 6 }, 
	};

#define  CHECK(a) if( (a) < 0) return (-1)

	static int findParamId(char *p_name, PARAM_TYPE *p_type)
	{
		PARAM_TABLE *p_tbl;

		if(!p_name || !p_type) return -1;		// Do some sanity check

		for(p_tbl = &ParamTable[0]; p_tbl->pName; p_tbl++)	// Search for the parameter name
		{
			if(!strcmp((const char *) p_tbl->pName, p_name)) 
			{
				*p_type = p_tbl->nType;
				return p_tbl->id;
			}
		}
		return -1;
	}

	void setParameter(int id_, float value_) {
		String8 str;
		char id[4] = {0,};
		char value[8] = {0,};

		str = "DSM=";
		snprintf(id, sizeof(id), "%d", id_);
		str += id;
		snprintf(value, sizeof(value), " %f", value_);
		str += value;
		AudioSystem::setParameters(0, str);
	}

	float getParameter(int id_) {
		String8 str;
		char id[4] = {0,};
		float value;

		str = "DSM=";
		snprintf(id, sizeof(id), "%d", id_);
		str += id;
		AudioParameter result(AudioSystem::getParameters(0, str));
		if (result.getFloat(String8("DSM"),value) == NO_ERROR) {
		} else
			value = -1;
		return value;
	}

	static int DSM_param_loop(void *state)
	{
		uint8_t		ch;
		uint32_t	set_flag;	// TRUE if Set, FALSE if Get
		uint8_t		ParamName[512];
		int			ParamValueInt;
		float		ParamValueFlt;
		double		ParamValueDbl;
		int             id;
		float p_Address;
		void		*p_Result;
		PARAM_TYPE	n_type;
		unsigned int max_size;

		while(1)
		{
			//  Get the first letter of Command
			CHECK(recv_cmd(state, &ch));
			if(ch != CP_APP_PARAM)
			{
				continue;
			}
			// Get the parameter Name and Get/Set flag
			CHECK(recv_param(state, &set_flag, &ParamName[0], sizeof(ParamName)));
			buff_cmd(state, CP_APP_PARAM);
			id = findParamId((char *)&ParamName[0], &n_type);	// Find the address and the type of the parameter
			if(set_flag)	// The parameter was specified as AAA.BBB.CCC=
			{
				if(id > 0)
				{
					printf("%s\n", ParamName);
					if(n_type == PARAM_FLT)
					{
						CHECK(recv_float(state, &ParamValueFlt));
						setParameter(id, ParamValueFlt);
						printf("<- %lf\n", (double)ParamValueFlt);
					}
					buff_int(state, 0);
				}else
				{
					printf("%s", ParamName);
					CHECK(recv_string(state, ParamName, sizeof(ParamName)));	// Just dump the parameter value
					printf(" (ERROR)<- \"%s\"\n", ParamName);
					buff_int(state, -2);	// Parameter is not found - error
				}
			} else {
				if(id > 0)
				{
					buff_int(state, 0);
					printf("%s", ParamName);
					if(n_type == PARAM_FLT)
					{
						p_Address = getParameter(id);
						buff_float(state,  p_Address);
						printf(" -> %f\n", p_Address);
					}
				}else
				{
					buff_int(state, -2);	// Parameter is not found - error
					printf("%s", ParamName);
					printf(" (ERROR)->\n");
				}
			}
			CHECK(buff_send(state));
		}
		return -1;	// Done - close connection
	}

	typedef struct dsm{
		int port_no;
		float value;
	}dsm;
	dsm dsm_t;

	static void usage(char *command)
	{
		printf(
				("Usage: %s [OPTION]... [FILE]...\n"
				 "\n"
				 "-h, --help              help\n"
				 "-p, --port           tcp port number\n"
				 "-S, --set            dsm param name\n"
				 "-V, --value            dsm param value\n"
				 "-G, --get            dsm param to read from\n"
				 "-t, --tcp_start     start the tcp reading\n")
				, command);
	}

	extern "C" {
		int main( int argc, char *argv[] )
		{
			int card, dev, tmp, err, c, id;
			char *command;
			char *dsm_param = "default";
			PARAM_TYPE *p_type;

			command = argv[0];
			if ( strstr( argv[0], "dsmconfig" ) ) {
				command = "dsmconfig";
			} else {
				fprintf( stderr, "Error: command should be named dsmconfig\n");
				return 1;
			}

			if ( argc > 1 && !strcmp( argv[1], "--help" ) ) {
				usage( command );
				return 0;
			}
			while ( (c = getopt( argc, argv, "htp:S:G:")) != EOF )
				switch ( c ) {
					case 'h':
						usage( command );
						return 0;
					case 'p':
						dsm_t.port_no = atoi( optarg );
						break;
					case 't':
						init_server(dsm_t.port_no, DSM_param_loop);
						start_server();
						break;
					case 'V':
						dsm_t.value = strtod(optarg,NULL);	
					case 'S':
						dsm_param = optarg;
						id = findParamId(dsm_param, &p_type);
						if (!(id < 0)) {
							setParameter(id, dsm_t.value);
						} else {
							printf("wrong command\n");
						}
					case 'G':
						dsm_param = optarg;
						id = findParamId(dsm_param, &p_type);
						if (!(id < 0)) {
							float value;
							value = getParameter(id);
							printf("%s is %f\n", dsm_param, value);
						} else {
							printf("wrong command\n");
						}
					default:
						usage( command );
						return 1;
				}
			fprintf( stderr, "%s\n", argv[2]);
			return 0;
		}
	}
}