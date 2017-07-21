#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h> //usleep

#include <arpa/inet.h> //inet_addr()
#include <fcntl.h> //'O_RDWR' & 'O_EXCL'
#include <unistd.h> //close
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/stat.h> //chmod

#if defined(__ANDROID_OS__)
#include <android/log.h>
#endif

#include "slp_api.h"

/*******************************************
*         DO NOT CHANGE THIS FILE          *
*  this is interface and callback function *
*  to/from SLPD for FLPD, AGPSD, and SLPD  *
*******************************************/

static void tag_log(int type, const char* tag, const char *fmt, ...);
#ifdef LOGD
#undef LOGD
#endif
#ifdef LOGW
#undef LOGW
#endif
#ifdef LOGE
#undef LOGE
#endif
#define LOGD(...) tag_log(0, "[slp][api]", __VA_ARGS__);
#define LOGW(...) tag_log(0, "[slp] WARNING: [api]", __VA_ARGS__);
#define LOGE(...) tag_log(1, "[slp] ERR: [api]", __VA_ARGS__);

//===============================================================
//get
static char get_byte(char* buff, int* offset) {
    LOGE("get byte");
    char ret = buff[*offset];
    *offset += 1;
    return ret;
}

static short get_short(char* buff, int* offset) {
    short ret = 0;
    LOGE("get short");
    ret |= get_byte(buff, offset) & 0xff;
    ret |= (get_byte(buff, offset) << 8);
    return ret;
}

static int get_int(char* buff, int* offset) {
    int ret = 0;
    LOGE("get int");
    ret |= get_short(buff, offset) & 0xffff;
    ret |= (get_short(buff, offset) << 16);
    return ret;
}
/*
static long long get_long(char* buff, int* offset) {
    long long ret = 0;
    ret |= get_int(buff, offset) & 0xffffffffL;
    ret |= ((long long)get_int(buff, offset) << 32);
    return ret;
}
*/
/*
static float get_float(char* buff, int* offset) {
    float ret;
    int tmp = get_int(buff, offset);
    ret = *((float*)&tmp);
    return ret;
}
*/
/*
static double get_double(char* buff, int* offset) {
    double ret;
    long long tmp = get_long(buff, offset);
    ret = *((double*)&tmp);
    return ret;
}
*/
/*
static char* get_string(char* buff, int* offset) {
    char ret = get_byte(buff, offset);
    if(ret == 0) {
        return NULL;
    } else {
        char* p = NULL;
        int len = get_int(buff, offset);
        p = &buff[*offset];
        *offset += len;
        return p;
    }
}
*/
/*
static char* get_string2(char* buff, int* offset) {
    char* output = get_string(buff, offset);
    if(output == NULL) {
        return "";
    } else {
        return output;
    }
}
*/
static int get_binary(char* buff, int* offset, char* output) {
    int len = get_int(buff, offset);
    if(len > 0) {
        memcpy(output, &buff[*offset], len);
        *offset += len;
    }
    return len;
}
//===============================================================
//put
static void put_byte(char* buff, int* offset, const char input) {
    *((char*)&buff[*offset]) = input;
    *offset += 1;
}

static void put_short(char* buff, int* offset, const short input) {
    put_byte(buff, offset, input & 0xff);
    put_byte(buff, offset, (input >> 8) & 0xff);
}

static void put_int(char* buff, int* offset, const int input) {
    put_short(buff, offset, input & 0xffff);
    put_short(buff, offset, (input >> 16) & 0xffff);
}
/*
static void put_long(char* buff, int* offset, const long long input) {
    put_int(buff, offset, input & 0xffffffffL);
    put_int(buff, offset, ((input >> 32L) & 0xffffffffL));
}
*/
/*
static void put_float(char* buff, int* offset, const float input) {
    int* data = (int*)&input;
    put_int(buff, offset, *data);
}
*/
/*
static void put_double(char* buff, int* offset, const double input) {
    long long* data = (long long*)&input;
    put_long(buff, offset, *data);
}
*/
static void put_string(char* buff, int* offset, const char* input) {
    if(input == NULL) {
        put_byte(buff, offset, 0);
    } else {
        int len = strlen(input) + 1;
        put_byte(buff, offset, 1);
        put_int(buff, offset, len);
        memcpy(&buff[*offset], input, len);
        *offset += len;
    }
}

static void put_binary(char* buff, int* offset, const char* input, const int len) {
    put_int(buff, offset, len);
    if(len > 0) {
        memcpy(&buff[*offset], input, len);
        *offset += len;
    }
}

//-1 means failure
static int get_time_str(char* buf, int len) {
    struct timeval  tv;
    struct timezone tz;
    struct tm      *tm;

    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);

    memset(buf, 0, len);
    sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d.%03d",
        tm->tm_year + 1900, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
        (int)(tv.tv_usec / 1000));

    return 0;
}

static void tag_log(int type, const char* tag, const char *fmt, ...) {
    char out_buf[1100] = {0};
    char buf[1024] = {0};
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    sprintf(out_buf, "%s %s", tag, buf);

#if !defined(__ANDROID_OS__)
    //Linux output
    char time_buf[64] = {0};
    get_time_str(time_buf, sizeof(time_buf));
    printf("%s %s\n", time_buf, out_buf);
#else
    if(type == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "slp", "%s", out_buf);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "slp", "%s", out_buf);
    }
#endif
}


//-1 means failure
int socket_bind_udp(const char* path) {
    int fd;
    struct sockaddr_un addr;

    fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if(fd < 0) {
        LOGE("socket_bind_udp() socket() failed reason=[%s]%d",
            strerror(errno), errno);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_path[0] = 0;
    memcpy(addr.sun_path + 1, path, strlen(path));
    addr.sun_family = AF_UNIX;
    unlink(addr.sun_path);
    if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOGE("socket_bind_udp() bind ()failed path=[%s] reason=[%s]%d",
            path, strerror(errno), errno);
        return -1;
    }
    chmod(path, 0660);
    return fd;
}

//-1 means failure
static int socket_udp_sendto(const char* path, const char* buff, int len) {
    int ret = 0;
    struct sockaddr_un addr;
    int retry = 10;
    int fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if(fd < 0) {
        LOGE("sock_udp_sendto() socket() failed reason=[%s]%d",
            strerror(errno), errno);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_path[0] = 0;
    memcpy(addr.sun_path + 1, path, strlen(path));
    addr.sun_family = AF_UNIX;

    while((ret = sendto(fd, buff, len, 0,
        (const struct sockaddr *)&addr, sizeof(addr))) == -1) {
        if(errno == EINTR) continue;
        if(errno == EAGAIN) {
            if(retry-- > 0) {
                usleep(100 * 1000);
                continue;
            }
        }
        LOGE("sock_udp_sendto() sendto() failed path=[%s] ret=%d reason=[%s]%d",
            path, ret, strerror(errno), errno);
        break;
    }

    close(fd);
    return ret;
}

//-1 means failure
static int socket_udp_recvfrom(int fd, void* buff, int len) {
    int ret = 0;
    int retry = 10;

    while((ret = recvfrom(fd, buff, len, 0, NULL, NULL)) == -1) {
        if(errno == EINTR) continue;
        if(errno == EAGAIN) {
            if(retry-- > 0) {
                usleep(100 * 1000);
                continue;
            }
        }
        LOGE("socket_udp_recvfrom() recvfrom() failed reason=[%s]%d",
            strerror(errno), errno);
        break;
    }
    return ret;
}

const char* get_sclp_result_enum_str(sclp_result_enum input) {
    switch(input) {
    case SCLP_RESULT_SUCCESS:
        return "SUCCESS";
    case SCLP_RESULT_ERROR:
        return "ERROR";
    case SCLP_RESULT_ERROR_WAIT_UPDATE:
        return "ERROR_WAIT_UPDATE";
    case SCLP_RESULT_ERROR_INVALID_PARAM:
        return "ERROR_INVALID_PARAM";
    case SCLP_RESULT_ERROR_QUEUE_IS_FULL:
        return "ERROR_QUEUE_IS_FULL";
    case SCLP_RESULT_ERROR_DNS_FAIL:
        return "ERROR_DNS_FAIL";
    case SCLP_RESULT_ERROR_CREATE_SOCKET_FAIL:
        return "ERROR_CREATE_SOCKET_FAIL";
    case SCLP_RESULT_ERROR_IN_SOCKET_CONNECTED:
        return "ERROR_IN_SOCKET_CONNECTED";
    case SCLP_RESULT_ERROR_IN_SUPL:
        return "ERROR_IN_SUPL";
    case SCLP_RESULT_ERROR_IN_TCP:
        return "ERROR_IN_TCP";
    case SCLP_RESULT_ERROR_IN_TLS:
        return "ERROR_IN_TLS";
    case SCLP_RESULT_ERROR_SOCKET_CONNECT_TIMEOUT:
        return "ERROR_SOCKET_CONNECT_TIMEOUT";
    case SCLP_RESULT_ERROR_TLS_HANDSHAKING_TIMEOUT:
        return "ERROR_TLS_HANDSHAKING_TIMEOUT";
    case SCLP_RESULT_ERROR_SUPL_UT1_TIMEOUT:
        return "ERROR_SUPL_UT1_TIMEOUT";
    case SCLP_RESULT_ERROR_SUPL_UT2_TIMEOUT:
        return "ERROR_SUPL_UT2_TIMEOUT";
    case SCLP_RESULT_ERROR_SUPL_UT3_TIMEOUT:
        return "ERROR_SUPL_UT3_TIMEOUT";
    case SCLP_RESULT_ERROR_NO_POSITION:
        return "ERROR_NO_POSITION";
    default:
        break;
    }
    return "UNKNOWN";
}

void dump_sclp_cell_info_list(sclp_cell_info_list* list) {
    int i = 0;
    LOGD("dump_sclp_cell_info_list() caller=[%d] session_id=[%d] num=[%d]",
        list->caller, list->session_id, list->num);
    for(i = 0; i < list->num; i++) {
        sclp_cell_info* cell = &list->cells[i];
        switch(cell->cell_type) {
            case SCLP_CELL_GSM: {
                sclp_gsm_cell* gsm = &cell->cell.gsm_cell;
                LOGD(" i=[%d] gsm  mcc=[%d] mnc=[%d] lac=[%d] cid=[%d]",
                    i, gsm->mcc, gsm->mnc, gsm->lac, gsm->cid);
                break;
            }
            case SCLP_CELL_WCDMA: {
                sclp_wcdma_cell* wcdma = &cell->cell.wcdma_cell;
                LOGD(" i=[%d] wcdma  mcc=[%d] mnc=[%d] lac=[%d] uc=[%d]",
                    i, wcdma->mcc, wcdma->mnc, wcdma->lac, wcdma->uc);
                break;
            }
            case SCLP_CELL_LTE: {
                sclp_lte_cell* lte = &cell->cell.lte_cell;
                LOGD(" i=[%d] lte  mcc=[%d] mnc=[%d] tac=[%d] ci=[%d] pci=[%d]",
                    i, lte->mcc, lte->mnc, lte->tac, lte->ci, lte->pci);
                break;
            }
            case SCLP_CELL_C2K: {
                sclp_cdma_cell* cdma = &cell->cell.cdma_cell;
                LOGD(" i=[%d] cdma  sid=[%d] nid=[%d] bid=[%d]",
                    i, cdma->sid, cdma->nid, cdma->bid);
                break;
            }
        }
    }
}

void dump_sclp_cell_location_list(sclp_cell_location_list* list) {
    int i = 0;
    LOGD("dump_sclp_cell_location_list() session_id=[%d] num=[%d]",
        list->session_id, list->num);
    for(i = 0; i < list->num; i++) {
        sclp_cell_location* l = &list->locations[i];
        LOGD(" i=[%d] index=[%d] result=[%s] lat=[%f] lng=[%f] alt=[%d] acc=[%d]",
            i, l->index, get_sclp_result_enum_str(l->result), l->lat, l->lng, l->alt, l->acc);
    }
}

//-1 means failure
int slp_start(sclp_cell_info_list* list) {
    int ret = 0;
    char buff[SCLP_CMD_BUFF_SIZE] = {0};
    int offset = 0;

    put_int(buff, &offset, SCLP_CMD_START);
    put_binary(buff, &offset, (const char*)list, sizeof(sclp_cell_info_list));
    ret = socket_udp_sendto(SCLP_CMD_SOCKET_PATH, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_start() fail");
    }
    return ret;
}

//-1 means failure
int slp_start2(const char* addr, int port, int tls_enabled, sclp_cell_info_list* list) {
    int ret = 0;
    char buff[SCLP_CMD_BUFF_SIZE] = {0};
    int offset = 0;

    put_int(buff, &offset, SCLP_CMD_START_2);
    put_string(buff, &offset, addr);
    put_int(buff, &offset, port);
    put_int(buff, &offset, tls_enabled);
    put_binary(buff, &offset, (const char*)list, sizeof(sclp_cell_info_list));
    ret = socket_udp_sendto(SCLP_CMD_SOCKET_PATH, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_start2() fail");
    }
    return ret;
}

//-1 means failure
int slp_start3(sclp_config* config, sclp_cell_info_list* list) {
    int ret = 0;
    char buff[SCLP_CMD_BUFF_SIZE] = {0};
    int offset = 0;

    put_int(buff, &offset, SCLP_CMD_START_3);
    put_binary(buff, &offset, (const char*)config, sizeof(sclp_config));
    put_binary(buff, &offset, (const char*)list, sizeof(sclp_cell_info_list));
    ret = socket_udp_sendto(SCLP_CMD_SOCKET_PATH, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_start3() fail");
    }
    return ret;
}

//-1 means failure
int slp_config_profile(const char* addr, int port, char tls_enable) {
    int ret = 0;
    sclp_config config;
    memset(&config, 0, sizeof(config));
    config.has_profile = 1;
    strcpy(config.profile.addr, addr);
    config.profile.port = port;
    config.profile.tls_enable = tls_enable;
    ret = slp_config(&config);
    if(ret < 0) {
        LOGE("slp_config_profile() fail");
    }
    return ret;
}

//-1 means failure
int slp_config_supl_version(int version) {
    int ret = 0;
    sclp_config config;
    memset(&config, 0, sizeof(config));
    config.has_supl_version = 1;
    config.supl_version = version;
    ret = slp_config(&config);
    if(ret < 0) {
        LOGE("slp_config_supl_version() fail");
    }
    return ret;
}

//-1 means failure
int slp_config_tls_version(int version) {
    int ret = 0;
    sclp_config config;
    memset(&config, 0, sizeof(config));
    config.has_tls_version = 1;
    config.tls_version = version;
    ret = slp_config(&config);
    if(ret < 0) {
        LOGE("slp_config_tls_version() fail");
    }
    return ret;
}

//-1 means failure
int slp_config(sclp_config* config) {
    int ret = 0;
    char buff[SCLP_CMD_BUFF_SIZE] = {0};
    int offset = 0;

    put_int(buff, &offset, SCLP_CMD_CONFIG);
    put_binary(buff, &offset, (const char*)config, sizeof(sclp_config));
    ret = socket_udp_sendto(SCLP_CMD_SOCKET_PATH, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_config() fail");
    }
    return ret;
}

//-1 means failure
int slp_reset() {
    int ret = 0;
    char buff[SCLP_CMD_BUFF_SIZE] = {0};
    int offset = 0;

    put_int(buff, &offset, SCLP_CMD_RESET);
    ret = socket_udp_sendto(SCLP_CMD_SOCKET_PATH, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_reset() fail");
    }
    return ret;
}

//-1 means failure
int slp_bind_callback(sclp_caller caller) {
    char caller_path[64] = {0};
    sprintf(caller_path, "%s_%d", SCLP_CALLBACK_SOCKET_PATH, caller);
    return socket_bind_udp(caller_path);
}

void slp_callback_hdlr(int fd, int * type, sclp_cell_location_list * list) {
    char buff[SCLP_CALLBACK_BUFF_SIZE] = {0};
    int offset = 0;
    int ret = 0;

    LOGE("slp_callback_hdlr function call");
    ret = socket_udp_recvfrom(fd, buff, sizeof(buff));
    if(ret < 0) {
        LOGE("slp_callback_hdlr() socket_udp_recvfrom() fail");
        return;
    }
    else
    {
        LOGE("slp_callback_hdlr() socket_udp_recvfrom() success");
    }

    sclp_callback_enum cmd = get_int(buff, &offset);
    switch(cmd) {
        case SCLP_CALLBACK_REBOOT:
            LOGE("enter SCLP_CALLBACK_REBOOT");
            *type = (int)SCLP_CALLBACK_REBOOT;
            list = NULL;
            break;
        case SCLP_CALLBACK_LOCATION_RESULT:
            {
            LOGE("enter SCLP_CALLBACK_LOCATION_RESULT");
            get_binary(buff, &offset, (char*)list);
            *type = (int)SCLP_CALLBACK_LOCATION_RESULT;
            break;
        }
        default: {
            LOGE("slp_callback_hdlr() unknown cmd=%d", cmd);
            break;
        }
    }
}

