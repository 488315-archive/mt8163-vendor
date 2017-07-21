#ifndef _VOW_API_AP_H
#define _VOW_API_AP_H

#ifdef __cplusplus
extern "C" {
#endif


struct VOW_TestInfo {
    int mode;  // keyword (1) or keyword+SV (2)
    int modelType;  // load SModel (0) or IModel (1)
    int commandID;
    const char *path;
    int rtnModelSize;
    char *rtnModel;
};

struct VOW_SoungtriggerTestInfo {
    char * rawModelFilePtr;
    int rawModelFileSizeInByte;
    int rtnModelSize;
    char *rtnModel;

};

int getSizes (struct VOW_SoungtriggerTestInfo *vowInfo);
int TestingInitAP (struct VOW_SoungtriggerTestInfo *vowInfo);


#ifdef __cplusplus
}
#endif

#endif
