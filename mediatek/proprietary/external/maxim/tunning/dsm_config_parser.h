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

/*
* need add config for DSM lib
*/
#include "dsm_api.h"
#include "dsm_api_types.h"

void dsm_configure(int usecase, void * dsm_handler );

