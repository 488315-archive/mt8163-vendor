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
#include <tinyxml.h>

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

#ifndef TO_FIX
#define TO_FIX(a, q) 	((int)((a) * ((unsigned int)1<<(q))))
#endif

#ifndef TO_DBL
#define TO_DBL(a, q)   (double)((double)((double)(a)/(double)((unsigned int)1<<(q))))
#endif

#define DSM_SET_STEREO_PARAM(cmdId)      ((cmdId&0x00FFFFFF)|0x03000000)
#define DSM_SET_MONO_PARAM(cmdId)      ((cmdId&0x00FFFFFF))


#define TOTAL_DSM_PARAMS 200

namespace android {
    typedef enum PARAM_TYPE { PARAM_NONE, PARAM_INT, PARAM_FLT, PARAM_DBL, PARAM_STR, PARAM_EXE_INT, PARAM_EXE_FLT, PARAM_EXE_DBL, PARAM_EXE_STR } PARAM_TYPE;

    typedef struct PARAM_TABLE{
        char		pName[50];		// The name of the parameter
        PARAM_TYPE      nType;      // Parameter type - int, float, double, string, function
        int             id;             //command id
        int             qType;             //q type
        double          cValue;     //current value
    }PARAM_TABLE;

    typedef struct firmware_info{
        char p_name[50];
        char p_value[50];
    }dsm_fm_info;

    static dsm_fm_info dsm_info[] = {
        {"GET_FIRMWARE_BUILD_TIME", "21:22:47"},
        {"GET_FIRMWARE_BUILD_DATE", "Jul 7 2015"},
        {"GET_FIRMWARE_VERSION", "7689434fa22342"},
        {"GET_CHIPSET_MODEL", "A56"},
    };
    static PARAM_TABLE params[TOTAL_DSM_PARAMS];
    static int total_parms = -1;

#define  CHECK(a) if( (a) < 0) return (-1)

    static int findParamId(int id)
    {
        int i;
        for (i=1;i<total_parms;i++)
            if (id == params[i].id) {
                return i;
            }
        return -1;
    }

    void setParameter(int id_, int value_) {
        String8 str;
        char id[32] = {0,};
        char value[20] = {0,};

        str = "DSM=";
        snprintf(id, sizeof(id), "%d", id_);
        printf("setParameter id_ = %d value %d\n",id_, value_);
        str += id;
        snprintf(value, sizeof(value), " %d", value_);
        str += value;
        const char *string = str;
        AudioSystem::setParameters(0, str);
    }

    int getParameter(int id_) {
        String8 str;
        char id[4] = {0,};
        String8 value;

        str = "DSM=";
        snprintf(id, sizeof(id), "%d", id_);
        str += id;
        const char *string = str;
        AudioParameter result(AudioSystem::getParameters(0, str));
        if (result.get(String8("DSM"),value) == NO_ERROR) {
            const char *change_int = value;
            return atoi(change_int);
        } else
            return -1;
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
            //id = findParamId((char *)&ParamName[0]);	// Find the address and the type of the parameter
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
        double value;
    }dsm;
    dsm dsm_t;

    static void usage(char *command)
    {
        printf(
                ("Usage: %s [OPTION]... [FILE]...\n"
                 "\n"
                 "-h, --help              help\n"
                 "-p, --port           tcp port number\n"
                 "-v, --verbose           show all params\n"
                 "-w, --write          write id value (./data/dsm_ctrl -w id value)\n"
                 "-r, --read            dsm param to read from, 0 will cause the dump of all the values (./data/dsm_ctrl -r id)\n"
                 "-t, --tcp_start     start the tcp reading\n")
                , command);
        printf("\ncontact anish.kumar@maximintegrated.com for any bugs\n");
    }

    void dsm_fill() {
        TiXmlDocument doc("/system/etc/DSM.xml");
        bool loadOkay = doc.LoadFile(); // Error checking in case file is missing
        if(loadOkay)
        {
            int i = 1;
            TiXmlElement *pRoot = doc.RootElement();
            TiXmlElement *element = pRoot->FirstChildElement();
            while(element)
            {
                TiXmlElement *elementChild;
                params[i].id = atoi(element->Attribute("id"));
                strcpy(params[i].pName, (char *)element->Attribute("name"));
                elementChild = element->FirstChildElement();
                params[i].nType = (PARAM_TYPE)atoi(elementChild->Attribute("type"));
                params[i].qType = atoi(elementChild->Attribute("value"));
                params[i].cValue = atoi(elementChild->Attribute("currentv"));
                element = element->NextSiblingElement();
                i++;
            }
            total_parms = i;
        }
    }

    void dsm_update(char *p_attr) {
        TiXmlDocument doc("/data/DSM.xml");
        bool loadOkay = doc.LoadFile(); // Error checking in case file is missing
        if(loadOkay)
        {
            int i = 1;
            TiXmlElement *pRoot = doc.RootElement();
            TiXmlElement *element = pRoot->FirstChildElement();
            while(element)
            {
                TiXmlElement *elementChild;
                elementChild = element->FirstChildElement();
                if (!strcmp(p_attr, (char *)element->Attribute("name"))) {
                    char int_to_str[50];
                    sprintf(int_to_str, "%lf", params[i].cValue);
                    elementChild->SetAttribute("currentv", int_to_str);
                    doc.SaveFile();
                }
                element = element->NextSiblingElement();
                i++;
            }
        }
    }

    double get_param_value(int id)
    {
        int i;
        double value = 0;
        double ret = 0;
        /*find the index from the id */
        i = findParamId(id);
        if (i < 0) {
            printf("wrong command\n");
            return -1;
        }

        /* special case when qType is character type */
        if (params[i].qType != -1) {
            value = getParameter(params[i].id);
            ret = TO_DBL(value, params[i].qType);
            params[i].cValue = ret;
            dsm_update(params[i].pName);
        }
        return ret;
    }

    void show_params()
    {
        int i;

        for (i=1;i<total_parms;i++) {
            get_param_value(i);
        }
        fprintf( stderr, "ID\t\t\tNAME\t\t\tVALUE\n");
        for (i=1;i<total_parms;i++) {
            fprintf( stderr, "%2d\t%30s\t", params[i].id, params[i].pName);
            if (params[i].qType == -1) {
                unsigned int j;
#define SIZE(x) sizeof(x)/sizeof(x[0])
                for (j=0;j<SIZE(dsm_info);j++) {
                    if (!strcmp(params[i].pName, dsm_info[j].p_name)) {
                        fprintf( stderr, "%20s\n", dsm_info[j].p_value);
                    }
                }
            } else
                fprintf( stderr, "%20f\n", params[i].cValue);
        }
        dsm_fill();
    }

    extern "C" {
        int main( int argc, char *argv[] )
        {
            int card, dev, tmp, err, c, id=-1, i;
            char *command, *token;
            char dsm_param[100];
            PARAM_TYPE p_type;
            const char seprator[2] = " ";
            double value = 0;
            command = argv[0];
            fprintf(stderr , "main \n");
            printf("main \n");

            if ( strstr( argv[0], "dsm_ctrl" ) ) {
                command = "dsm_ctrl";
            } else {
                fprintf( stderr, "Error: command should be named dsm_ctrl\n");
                return 1;
            }

            if ( argc <= 1 ) {
                usage( command );
                return 0;
            }

            if ( argc > 1 && !strcmp( argv[1], "--help" ) ) {
                usage( command );
                return 0;
            }
            dsm_fill();
            while ( (c = getopt( argc, argv, "vhtp:w:r:V:")) != EOF )
                switch ( c ) {
                    case 'h':
                        usage( command );
                        return 0;
                    case 'r':
                        id = atoi(optarg);
                        if (id > 0 && id < total_parms) {
                            int index;
                            value = get_param_value(id);
                            index = findParamId(id);
                            printf("%s value is %lf\n", params[index].pName, value);
                            break;
                        }
                    case 'v':
                        if (!id)
                            show_params();
                        else if (id < 0 || id >= total_parms)
                            printf("wrong command, please use -h to see the options\n");
                        return 0;
                    case 'p':
                        dsm_t.port_no = atoi( optarg );
                        break;
                    case 't':
                        init_server(dsm_t.port_no, DSM_param_loop);
                        start_server();
                        break;
                    case 'w':
                        optind--;
						if (argc > 4) {
							printf("please check the usage\n");
							break;
						}
                        for( ;optind < argc; optind++){
                            if (id < 0)
                                id = atoi(argv[optind]);
                            else
                                value = atof(argv[optind]);
                        }
                        i = findParamId(id);
                        if (i > 0 && i < total_parms) {
                            int ret;
                            ret = TO_FIX(value, params[i].qType);
                            setParameter(DSM_SET_STEREO_PARAM(params[i].id), ret);
                            printf("%s is set to %lf\n", params[i].pName, value);
                        } else {
                            printf("wrong command, please use -h to see the options\n");
                        }
                        break;
                    default:
                        usage( command );
                        return 1;
                }
            return 0;
        }
    }
}
