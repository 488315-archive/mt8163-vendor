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
#include "dsm_config_parser.h"


typedef struct params
{
    int id;
    char name[50];
    int qValue;
    float cValue;
} dsm_param;

void dsm_configure_channels(int channels)
{

}


void dsm_configure(int usecase, void *dsm_handler)
{
    ALOGD("%s usecase = %d", __FUNCTION__, usecase);
    int id_to_q[256] =
    {
        0, 0,  19, 27, 30, 29, 27, 30, 19, 31, 27, 9,
        30, 9,  29, 0,  9,  29, 27, 19, 27, 0,  0,  0,
        0,  27, 27, 0,  0,  0,  0,  0,  9,  29, 20, 0,
        0,  0,  28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 0,  0,  0,  0,  0,  0,  0,  0,  28,
        28, 28, 28, 28, 20, 20, 24, 23, 28, 30, 30, 30,
        30, 0,  0,  0,  -1, -1, -1, -1, 0,  15, 0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  31, 0,
        0,  0,  31, 0
    };

    TiXmlDocument doc("/system/etc/DSM_config.xml");
    bool loadOkay = doc.LoadFile(); // Error checking in case file is missing
    if (loadOkay)
    {

#define DSM_SET_STEREO_PARAM(cmdId)         ((cmdId&0x00FFFFFF)|DSM_API_STEREO_SPKER)
#define DSM_SET_MONO_PARAM(cmdId)           ((cmdId&0x00FFFFFF)|DSM_API_MONO_SPKER)

        TiXmlElement *pRoot = doc.RootElement();
        dsm_param *params = (dsm_param *)malloc(sizeof(*params) * 50);
        TiXmlElement *element = pRoot->FirstChildElement();
        while (element)
        {
            int i = 0;
            TiXmlElement *elementChild;
            if (usecase == atoi(element->Attribute("usecase")))
            {
                elementChild = element->FirstChildElement();
                while (elementChild)
                {
                    params[i].id = atoi(elementChild->Attribute("id"));
                    params[i].qValue = id_to_q[params[i].id];
                    params[i].cValue = atof(elementChild->Attribute("currentv"));
                    elementChild = elementChild->NextSiblingElement();
                    i++;
                }
            }
            if (i)
            {
                int cmdBlk[50], j, k = 0;
                for (j = 0; j < i * 2; j += 2)
                {
                    cmdBlk[j] = DSM_SET_STEREO_PARAM(params[k].id);
#define TO_FIX(a, q)   ((int)((a) * ((unsigned int)1<<(q))))
                    cmdBlk[j + 1] = TO_FIX(params[k].cValue, params[k].qValue);
                    ALOGD("%s cvalue %d qvalue %d", __func__, params[k].cValue, params[k].qValue);
                    ALOGD("%s value %d", __func__, params[k].cValue * (1 << params[k].qValue));
                    ALOGD("cmdBlk[j] () = %d errcmdBlk[j + 1]  %d\n", (int)cmdBlk[j] , (int)cmdBlk[j] + 1);
                    k++;
                }
                int retCode = DSM_API_Set_Params((void *)dsm_handler, i, cmdBlk);
                if (DSM_API_OK != retCode)
                {
                    ALOGD("DSM_API_Set_Params() error code = %d\n", (int)retCode);
                }
                free(params);
                break;
            }
            element = element->NextSiblingElement();
        }
    }
}

