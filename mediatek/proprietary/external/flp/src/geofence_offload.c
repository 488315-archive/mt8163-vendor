#include <hardware/fused_location.h>
//#include <cutils/xlog.h>
//#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
//#include <sys/epoll.h>
#include <math.h>
//#include <hardware/gps.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/epoll.h>
#include <string.h>
#include <signal.h>
#include <time.h>
//#include <sys/un.h>
#include <sys/stat.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
#include <stdio.h>
//#include "ptype.h"
//#include "Geofence.h"
//#include "mtk_flp.h"
#include "mtk_flp_sys.h"
#include "mtk_flp_sys_type.h"


#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "OL_GEO"

#define FLPE(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLPD(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define FLPW(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_WARN, LOG_TAG, __VA_ARGS__)

#else
#define FLPD(...)
#define FLPW(...)
#define FLPE(...)
#endif

typedef enum
{
    inside,
    outside,
    uncertain
}SET_STATE;


typedef enum
{
    INIT_GEOFENCE,
    ADD_GEOFENCE_AREA,
    PAUSE_GEOFENCE,
    RESUME_GEOFENCE,
    REMOVE_GEOFENCE,
    MODIFY_GEOFENCE,
    //SET_GEOFENCE_LOC_CB,
    SET_GEOFENCE_PASSTHRU_CB,
    SET_GEOFENCE_CLEAR_CB,
}MTK_COMMAND_T;

typedef struct mtk_geofence_area
{
    int32_t geofence_id;
    double latitude;
    double longitude;
    double radius;
    int last_transition; /*current state, most cases is GPS_GEOFENCE_UNCERTAIN*/
    int monitor_transition; /*bitwise or of  entered/exited/uncertain*/
    int notification_period;/*timer  interval, period of report transition status*/
    int unknown_timer;/*continue positioning time limitied while positioning*/
    int alive;/*geofence status, 1 alive, 0 sleep*/
    uint32_t source_to_use; /* source to use for geofence */
    SET_STATE latest_state;/*latest status: outside, inside, uncertain*/
}MTK_GEOFENCE_PROPERTY_T;

typedef struct mtk_modify_geofence
{
    int32_t geofence_id;
    uint32_t source_to_use; /* source to use for geofence */
    int last_transition; /*current state, most cases is GPS_GEOFENCE_UNCERTAIN*/
    int monitor_transition; /*bitwise or of  entered/exited/uncertain*/
    int notification_period;/*timer  interval, period of report transition status*/
    int unknown_timer;/*continue positioning time limitied while positioning*/
}MTK_MODIFY_GEOFENCE;


#define FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE  4

static void mtk_flp_ofl_geofence_init(FlpGeofenceCallbacks* callbacks );
static void mtk_flp_ofl_geofence_add_geofences(int32_t number_of_geofences, Geofence** geofences);
static 	void mtk_flp_ofl_geofence_pause_geofence(int32_t geofence_id);
static 	void mtk_flp_ofl_geofence_resume_geofence(int32_t geofence_id, int monitor_transitions);
static void mtk_flp_ofl_geofence_modify_geofence_option(int32_t geofence_id, GeofenceOptions* options);
static void mtk_flp_ofl_geofence_remove_geofences(int32_t number_of_geofences, int32_t* geofence_id);

FlpGeofenceCallbacks *mtk_geofence_callbacks = NULL; /*keep callback function addresses*/
extern int g_server_socket_fd;


const FlpGeofencingInterface mtkFlpGeofenceInterface_ofl = {
    sizeof(FlpGeofencingInterface),
    mtk_flp_ofl_geofence_init,
    mtk_flp_ofl_geofence_add_geofences,
    mtk_flp_ofl_geofence_pause_geofence,
    mtk_flp_ofl_geofence_resume_geofence,
    mtk_flp_ofl_geofence_modify_geofence_option,
    mtk_flp_ofl_geofence_remove_geofences,
};



/*********************************************************/
/* FLP Geofence Interface implementation                 */
/*********************************************************/

void mtk_flp_ofl_geofence_init(FlpGeofenceCallbacks* callbacks )
{
    int GeoCapability = 0;
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;

    FLPD("init geofence, cb function : %x",callbacks);
    mtk_geofence_callbacks = callbacks;

    GeoCapability = (int)(CAPABILITY_GNSS | CAPABILITY_WIFI | CAPABILITY_CELL);
    mtk_geofence_callbacks->flp_capabilities_cb(GeoCapability);

    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T);
    geo_msg.type = INIT_GEOFENCE;
    geo_msg.length = sizeof(MTK_FLP_MSG_T);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );

    mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
    //mtk_flp_dc_offload_process(flp_msg);

	return;
}


/*buffer format is like below: let's add fence batches
--------------------------------------------------------------
-command header---fence 1---fence 2 --- fence number_of_geofence--
---------------------------------------------------------------
*/
void mtk_flp_ofl_geofence_add_geofences(int32_t number_of_geofences, Geofence** geofences)
{
    int i;
    int32_t partial_size;
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;
    MTK_GEOFENCE_PROPERTY_T dbg_fence;

    FLPD("mtk_flp_geofence_add_geofences, add %d fence",number_of_geofences);

#if 0
    //Twice times geofence setting sending to connsys if fence no>4
    // 1 fence = 18byte Geofence + 4byte number_of_geofences + 2header 16byte = 38bytes
    // 8fences = 144byte Geofence + 4byte number_of_geofences + 2header 16byte = 164bytes
    // current connsys recv msg len restricted to 126bytes
    // Till 2014/12/12 if fence no >4, separate the msg into 2 (msg1: fence 1-4), (msg2: fence 5-8)

    if(number_of_geofences > FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE)
    {
        //Send first half
        flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_FLP_MSG_T) + sizeof(int32_t) + FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence));
        flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
        flp_msg->length = sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t) + FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence);
        geo_msg.type = ADD_GEOFENCE_AREA;
        geo_msg.length = sizeof(int32_t) + FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence);

        memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &number_of_geofences, sizeof(int32_t) );
        dbg_fence = mtk_flp_sys_msg_alloc( FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence));
        memcpy(dbg_fence,(*geofences),FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence));
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)),dbg_fence, FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE*sizeof(Geofence) );
        mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);

        //Send second half
        partial_size = number_of_geofences-FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE;
        flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_FLP_MSG_T) + sizeof(int32_t) + partial_size*sizeof(Geofence));
        flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
        flp_msg->length = sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t) + partial_size*sizeof(Geofence);
        geo_msg.type = ADD_GEOFENCE_AREA;
        geo_msg.length = sizeof(int32_t) + partial_size*sizeof(Geofence);

        memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &partial_size, sizeof(int32_t) );
        dbg_fence = mtk_flp_sys_msg_alloc( partial_size*sizeof(Geofence));
        memcpy(dbg_fence,(*geofences)+FLP_SEND_CONNSYS_MAX_FENCE_AT_ONCE,partial_size*sizeof(Geofence));
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)),dbg_fence, partial_size*sizeof(Geofence) );
        mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
    }
    else
#endif
    {
        //Send the geofence data one by one
        for (i=0;i<number_of_geofences;i++)
        {
           flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_FLP_MSG_T) + sizeof(int32_t) + sizeof(MTK_GEOFENCE_PROPERTY_T));
           flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
           flp_msg->length =  sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t) + sizeof(MTK_GEOFENCE_PROPERTY_T);
           geo_msg.type = ADD_GEOFENCE_AREA;
           geo_msg.length =  sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t) + sizeof(MTK_GEOFENCE_PROPERTY_T);
           memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
           memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &number_of_geofences, sizeof(int32_t) );
           memset(&dbg_fence,0,sizeof(MTK_GEOFENCE_PROPERTY_T));
           dbg_fence.geofence_id = geofences[i]->geofence_id;
           dbg_fence.latitude= geofences[i]->data->geofence.circle.latitude;
           dbg_fence.longitude = geofences[i]->data->geofence.circle.longitude;
           dbg_fence.radius = geofences[i]->data->geofence.circle.radius_m;
           dbg_fence.last_transition = geofences[i]->options->last_transition;
           dbg_fence.monitor_transition= geofences[i]->options->monitor_transitions;
           dbg_fence.notification_period= geofences[i]->options->notification_responsivenes_ms;
           dbg_fence.unknown_timer= geofences[i]->options->unknown_timer_ms;
           dbg_fence.alive = 1;
           dbg_fence.source_to_use = geofences[i]->options->sources_to_use;
           dbg_fence.latest_state = uncertain;
           memcpy((((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)),&dbg_fence,sizeof(MTK_GEOFENCE_PROPERTY_T));
           FLPD("ofl: add fence,id=%ld,type=%d,%d,%d,%lf,%d,%d",dbg_fence.geofence_id,dbg_fence.last_transition,dbg_fence.monitor_transition,
            dbg_fence.notification_period,dbg_fence.latitude,dbg_fence.unknown_timer,dbg_fence.source_to_use);
           mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
        }
     }

    return;
}

void mtk_flp_ofl_geofence_pause_geofence(int32_t geofence_id)
{
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;
    INT8 *msg_dbg;

    FLPD("mtk_flp_geofence_pause_geofence, id: %d fence",geofence_id);

    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t);
    geo_msg.type = PAUSE_GEOFENCE;
    geo_msg.length = sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &geofence_id, sizeof(int32_t) );
    msg_dbg = (INT8*)flp_msg;
    FLPD("geo dbg 1: %d %d %d %d %d %d %d %d",*msg_dbg, *(msg_dbg+1),*(msg_dbg+2),*(msg_dbg+3),*(msg_dbg+4),*(msg_dbg+5),*(msg_dbg+6),*(msg_dbg+7));
    FLPD("geo dbg 2: %d %d %d %d %d %d %d %d",*(msg_dbg+8), *(msg_dbg+9),*(msg_dbg+10),*(msg_dbg+11),*(msg_dbg+12),*(msg_dbg+13),*(msg_dbg+14),*(msg_dbg+15));
    FLPD("geo dbg 3: %d %d %d %d ",*(msg_dbg+16), *(msg_dbg+17),*(msg_dbg+18),*(msg_dbg+19));
    mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
    //mtk_flp_dc_offload_process(flp_msg); //send HAL(AP)->Kernel(connsys) msg here
    return;
}

void mtk_flp_ofl_geofence_resume_geofence(int32_t geofence_id, int monitor_transitions)
{
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;
    INT8 *msg_dbg;

    FLPD("mtk_flp_geofence_resume_geofence, id: %d fence",geofence_id);

    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T) + sizeof(int32_t) + sizeof(int));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t) + sizeof(int);
    geo_msg.type = RESUME_GEOFENCE;
    geo_msg.length =  sizeof(MTK_FLP_MSG_T) + sizeof(int32_t) + sizeof(int);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &geofence_id, sizeof(int32_t) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)), &monitor_transitions, sizeof(int) );

    msg_dbg = (INT8*)flp_msg;
    FLPD("geo dbg 1: %d %d %d %d %d %d %d %d",*msg_dbg, *(msg_dbg+1),*(msg_dbg+2),*(msg_dbg+3),*(msg_dbg+4),*(msg_dbg+5),*(msg_dbg+6),*(msg_dbg+7));
    FLPD("geo dbg 2: %d %d %d %d %d %d %d %d",*(msg_dbg+8), *(msg_dbg+9),*(msg_dbg+10),*(msg_dbg+11),*(msg_dbg+12),*(msg_dbg+13),*(msg_dbg+14),*(msg_dbg+15));
    FLPD("geo dbg 3: %d %d %d %d %d %d ",*(msg_dbg+16), *(msg_dbg+17),*(msg_dbg+18),*(msg_dbg+19),*(msg_dbg+20),*(msg_dbg+21));
    mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
    //mtk_flp_dc_offload_process(flp_msg); //send HAL(AP)->Kernel(connsys) msg here
    return;
}

void mtk_flp_ofl_geofence_modify_geofence_option(int32_t geofence_id, GeofenceOptions* options)
{
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;
    MTK_MODIFY_GEOFENCE modify_fence;

    FLPD("mtk_flp_geofence_modify_geofence_option, id: %d fence",geofence_id);

    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_MODIFY_GEOFENCE));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_MODIFY_GEOFENCE);
    geo_msg.type = MODIFY_GEOFENCE;
    geo_msg.length = sizeof(MTK_FLP_MSG_T)+ sizeof(MTK_MODIFY_GEOFENCE);
    modify_fence.geofence_id = geofence_id;
    modify_fence.last_transition = options->last_transition;
    modify_fence.monitor_transition = options->monitor_transitions;
    modify_fence.notification_period = options->notification_responsivenes_ms;
    modify_fence.unknown_timer = options->unknown_timer_ms;
    modify_fence.source_to_use = options->sources_to_use;
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &modify_fence, sizeof(MTK_MODIFY_GEOFENCE) );

    mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);

    return;
}

void mtk_flp_ofl_geofence_remove_geofences(int32_t number_of_geofences, int32_t* geofence_id)
{
    MTK_FLP_MSG_T* flp_msg=NULL;
    MTK_FLP_MSG_T geo_msg;
    unsigned int i;
    int32_t local_id;

    FLPD("mtk_flp_geofence_remove_geofences, no of fence=%d",number_of_geofences);
#if 1
    INT8 *msg_dbg;
    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t)+ number_of_geofences*sizeof(int32_t));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+ number_of_geofences*sizeof(int32_t);
    geo_msg.type = REMOVE_GEOFENCE;
    geo_msg.length = sizeof(MTK_FLP_MSG_T) + sizeof(int32_t)+ number_of_geofences*sizeof(int32_t);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &number_of_geofences, sizeof(int32_t) );

    for(i=0;i<number_of_geofences;i++){
        local_id = *(geofence_id + i);
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+i*sizeof(int32_t)), &local_id, sizeof(int32_t) );
        FLPD("fence %d, id %d",number_of_geofences,local_id );
    }
#else
    INT8 *msg_dbg;
    double a = 0.0;

    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t)+ number_of_geofences*sizeof(int32_t)+sizeof(double));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+ number_of_geofences*sizeof(int32_t)+sizeof(int32_t)+sizeof(double);
    geo_msg.type = REMOVE_GEOFENCE;
    geo_msg.length = sizeof(int32_t)+ number_of_geofences*sizeof(int32_t)+sizeof(int32_t)+sizeof(double);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &number_of_geofences, sizeof(int32_t) );

    for(i=0;i<number_of_geofences;i++){
        local_id = *(geofence_id + i);
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+i*sizeof(int32_t)), &local_id, sizeof(int32_t) );
        FLPD("fence %d, id %d",number_of_geofences,local_id );
    }
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t)+number_of_geofences*sizeof(int32_t)), &a, sizeof(double) );

 /*
    INT8 *msg_dbg;
    flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(MTK_FLP_MSG_T)+ sizeof(int32_t)+ number_of_geofences*sizeof(int32_t));
    flp_msg->type = MTK_FLP_MSG_OFL_GEOFENCE_CMD;
    flp_msg->length = sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+ number_of_geofences*sizeof(int32_t);
    geo_msg.type = REMOVE_GEOFENCE;
    geo_msg.length = sizeof(int32_t)+ number_of_geofences*sizeof(int32_t);
    memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &geo_msg, sizeof(MTK_FLP_MSG_T) );
    memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)), &number_of_geofences, sizeof(int32_t) );

    for(i=0;i<number_of_geofences;i++){
        local_id = *(geofence_id + i);
        memcpy( (((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T)+sizeof(MTK_FLP_MSG_T)+sizeof(int32_t)+i*sizeof(int32_t)), &local_id, sizeof(int32_t) );
        FLPD("fence %d, id %d",number_of_geofences,local_id );
    }
*/
#endif
    msg_dbg = (INT8*)flp_msg;
    FLPD("geo dbg 1: %d %d %d %d %d %d %d %d",*msg_dbg, *(msg_dbg+1),*(msg_dbg+2),*(msg_dbg+3),*(msg_dbg+4),*(msg_dbg+5),*(msg_dbg+6),*(msg_dbg+7));
    FLPD("geo dbg 2: %d %d %d %d %d %d %d %d",*(msg_dbg+8), *(msg_dbg+9),*(msg_dbg+10),*(msg_dbg+11),*(msg_dbg+12),*(msg_dbg+13),*(msg_dbg+14),*(msg_dbg+15));
    FLPD("geo dbg 3: %d %d %d %d %d %d %d %d",*(msg_dbg+16), *(msg_dbg+17),*(msg_dbg+18),*(msg_dbg+19),*(msg_dbg+20),*(msg_dbg+21),*(msg_dbg+22),*(msg_dbg+23));
    mtk_flp_sys_socket_send(g_server_socket_fd, flp_msg);
    //mtk_flp_dc_offload_process(flp_msg); //send HAL(AP)->Kernel(connsys) msg here
    return;
}


