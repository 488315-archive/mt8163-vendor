#ifndef _GEOFENCE_H_
#define _GEOFENCE_H_

#ifndef K2_PLATFORM
#include "mtk_flp_sys_type.h"
#else
#include "mtk_flp_connsys_sys_type.h"
#include <time.h>
#include <stdint.h>
#endif


typedef enum
{
    inside,
    outside,
    uncertain
}SET_STATE;

typedef enum
{
#ifdef K2_PLATFORM
    INIT_GEOFENCE,
#endif
    ADD_GEOFENCE_AREA,
    PAUSE_GEOFENCE,
    RESUME_GEOFENCE,
    REMOVE_GEOFENCE,
    MODIFY_GEOFENCE,
    //SET_GEOFENCE_LOC_CB,
    SET_GEOFENCE_PASSTHRU_CB,
    SET_GEOFENCE_CLEAR_CB,
}MTK_COMMAND_T;

typedef enum
{
    GEOFENCE_ADD_CALLBACK,
    GEOFENCE_REMOVE_CALLBACK,
    GEOFENCE_PAUSE_CALLBACK,
    GEOFENCE_RESUME_CALLBACK
}GEOFENCE_CALLBACK_T;

typedef struct mtk_geofence_callback
{
    int32_t cb_id;
    int32_t geofence_id;
    int32_t result;
}MTK_GEOFENCE_CALLBACK_T;

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

typedef struct mtk_pause_geofence
{
    int32_t geofence_id;
}MTK_PAUSE_GEOFENCE_T;

typedef struct mtk_resume_geofence
{
    int32_t geofence_id;
    int transition;
}MTK_RESUME_GEOFENCE_T;

typedef struct mtk_remove_geofence
{
    int32_t geofence_id;
}MTK_REMOVE_GEOFENCE_T;

typedef struct mtk_modify_geofence
{
    int32_t geofence_id;
    uint32_t source_to_use; /* source to use for geofence */
    int last_transition; /*current state, most cases is GPS_GEOFENCE_UNCERTAIN*/
    int monitor_transition; /*bitwise or of  entered/exited/uncertain*/
    int notification_period;/*timer  interval, period of report transition status*/
    int unknown_timer;/*continue positioning time limitied while positioning*/
}MTK_MODIFY_GEOFENCE;

typedef struct mtk_geofence_timer
{
    timer_t start_timer;
    timer_t stop_timer;
}MTK_GEOFENCE_TIMER;

#ifdef K2_PLATFORM
typedef float          R4;      // 4 byte floating point
typedef double         R8;      // 8 byte floating point

#define FLP_GEOFENCE_TRANSITION_ENTERED     (1L<<0)
#define FLP_GEOFENCE_TRANSITION_EXITED      (1L<<1)
#define FLP_GEOFENCE_TRANSITION_UNCERTAIN   (1L<<2)

#define FLP_GEOFENCE_MONITOR_STATUS_UNAVAILABLE (1L<<0)
#define FLP_GEOFENCE_MONITOR_STATUS_AVAILABLE   (1L<<1)

#define FLP_RESULT_SUCCESS                       0
#define FLP_RESULT_ERROR                        -1
#define FLP_RESULT_INSUFFICIENT_MEMORY          -2
#define FLP_RESULT_TOO_MANY_GEOFENCES           -3
#define FLP_RESULT_ID_EXISTS                    -4
#define FLP_RESULT_ID_UNKNOWN                   -5
#define FLP_RESULT_INVALID_GEOFENCE_TRANSITION  -6


/** Flags to indicate which values are valid in a FlpLocation. */
typedef uint16_t FlpLocationFlags;
typedef int64_t FlpUtcTime;

typedef struct geofence_new_alarm
{
   UINT32 unknown_init_ttick;  //record the ttick of fence starting time
   INT32 unknown_elapsed_ttick;  //update the ttick elapsed time since the starting time
   int geofence_operating_mode; //[start, stop/ pause] = [0,1]
}GEOFENCE_NEW_ALARM_T;

/** Type of geofence */
typedef enum {
    TYPE_CIRCLE = 0,
} GeofenceType;

/** Circular geofence is represented by lat / long / radius */
typedef struct {
    double latitude;
    double longitude;
    double radius_m;
} GeofenceCircle;

/** Represents the type of geofence and data */
typedef struct {
    GeofenceType type;
    union {
        GeofenceCircle circle;
    } geofence;
} GeofenceData;

/** Geofence Options */
typedef struct {
   /**
    * The current state of the geofence. For example, if
    * the system already knows that the user is inside the geofence,
    * this will be set to FLP_GEOFENCE_TRANSITION_ENTERED. In most cases, it
    * will be FLP_GEOFENCE_TRANSITION_UNCERTAIN. */
    int last_transition;

   /**
    * Transitions to monitor. Bitwise OR of
    * FLP_GEOFENCE_TRANSITION_ENTERED, FLP_GEOFENCE_TRANSITION_EXITED and
    * FLP_GEOFENCE_TRANSITION_UNCERTAIN.
    */
    int monitor_transitions;

   /**
    * Defines the best-effort description
    * of how soon should the callback be called when the transition
    * associated with the Geofence is triggered. For instance, if set
    * to 1000 millseconds with FLP_GEOFENCE_TRANSITION_ENTERED, the callback
    * should be called 1000 milliseconds within entering the geofence.
    * This parameter is defined in milliseconds.
    * NOTE: This is not to be confused with the rate that the GPS is
    * polled at. It is acceptable to dynamically vary the rate of
    * sampling the GPS for power-saving reasons; thus the rate of
    * sampling may be faster or slower than this.
    */
    int notification_responsivenes_ms;

   /**
    * The time limit after which the UNCERTAIN transition
    * should be triggered. This paramter is defined in milliseconds.
    */
    int unknown_timer_ms;

    /**
     * The sources to use for monitoring geofences. Its a BITWISE-OR
     * of FLP_TECH_MASK flags.
     */
    uint32_t sources_to_use;
} GeofenceOptions;

/** Represents a location. */
typedef struct {
    /** set to sizeof(FlpLocation) */
    size_t          size;

    /** Flags associated with the location object. */
    FlpLocationFlags flags;

    /** Represents latitude in degrees. */
    double          latitude;

    /** Represents longitude in degrees. */
    double          longitude;

    /**
     * Represents altitude in meters above the WGS 84 reference
     * ellipsoid. */
    double          altitude;

    /** Represents speed in meters per second. */
    float           speed;

    /** Represents heading in degrees. */
    float           bearing;

    /** Represents expected accuracy in meters. */
    float           accuracy;

    /** Timestamp for the location fix. */
    FlpUtcTime      timestamp;

    /** Sources used, will be Bitwise OR of the FLP_TECH_MASK bits. */
    uint32_t         sources_used;
} FlpLocation;

/** Geofence struct */
typedef struct {
    int32_t geofence_id;
    GeofenceData* data;
    GeofenceOptions* options;
} Geofence;

#else //un-offload alarm
typedef struct geofence_alarm
{
    struct itimerspec expire;
    int idx;
    struct sigevent evt;
    timer_t fd;
}GEOFENCE_ALARM_T;

#endif //ifdef offload

typedef enum
{
    START,
    STOP,
    UNKNOW
}MTK_GEOFENCE_ACTION;

static int start_unknown_monitor(const int item);

#endif
