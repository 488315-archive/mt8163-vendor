#define LOG_TAG "AudioSetParam"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#include <media/AudioSystem.h>
#include <media/mediaplayer.h>
#include <system/audio_policy.h>
#include <hardware/audio_policy.h>
#include <hardware_legacy/AudioPolicyInterface.h>
#include <hardware_legacy/AudioSystemLegacy.h>
#include <hardware/hardware.h>
#include <system/audio.h>

using namespace android;

static char key_aurisys[] = "AURISYS";

static void usage(const char *name);

static void getCommands();
static void getOptCommands(int argc, char *argv[]);
static void dispatchCommand(const char *cmd);

static void runSetParam(const char *cmd);
static void runGetParam(const char *cmd);



int main(int argc, char *argv[])
{
    ProcessState::self()->startThreadPool();
    sp<ProcessState> proc(ProcessState::self());

    if (argc == 1) { // interaction mode
        getCommands();
    }
    else if (argc == 2) { // single mode
        dispatchCommand(argv[1]);
    }
    else { // arg mode
        getOptCommands(argc, argv);
    }

    return 0;
}

static void getCommands()
{
    char cmd[1024];

    while (true) {
        printf("\nplease enter command, ex: 'GET_XXX_ENABLE', 'SET_XXX_ENABLE=0', 'SET_XXX_ENABLE=1', and '0' for exit\n\n");
        if (fgets(cmd, 1024, stdin) == NULL) {
            break;
        }

        if (cmd[0] == '0') { // exit
            break;
        }
        dispatchCommand(cmd);
    }
}


static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-s parameter=value] [-g parameter]\n", name);
    fprintf(stderr, "    -s    set parameter\n");
    fprintf(stderr, "    -g    get parameter\n");
    fprintf(stderr, "If no options, it will run in the command line mode.'\n");
}


static void getOptCommands(int argc, char *argv[])
{
    const char *const progname = argv[0];
    int opt = 0;

    while ((opt = getopt(argc, argv, "s:g:")) != -1) {
        switch (opt) {
            case 's':
                if (strchr(optarg, '=') == NULL) { // no '=', it's not set function
                    usage(progname);
                    goto EXIT;
                }
                runSetParam(optarg);
                break;
            case 'g':
                if (strrchr(optarg, '=') != NULL) { // has '=', it's set function
                    usage(progname);
                    goto EXIT;
                }
                runGetParam(optarg);
                break;
            default:
                usage(progname);
                goto EXIT;
        }
    }

EXIT:
    return;
}


static void dispatchCommand(const char *cmd)
{
    // remove '\n'
    char *ptr_newline = strchr(cmd, '\n');
    if (ptr_newline != NULL) {
        *ptr_newline = '\0';
    }

    if (strchr(cmd, '=') != NULL) { // has '=', it's set function
        runSetParam(cmd);
    }
    else {
        runGetParam(cmd);
    }
}


static void runSetParam(const char *cmd)
{
    status_t ret = NO_ERROR;

    ret = AudioSystem::setParameters(0, String8(cmd));
    //printf("ret: %d\n", ret);

    if (strncmp(cmd, key_aurisys, sizeof(key_aurisys) - 1) == 0) {
#if 0 // TODO: fix it
        printf("%s\n", (ret == 0) ? "SET_PASS" : "SET_FAIL");
#else
        printf("%s\n",
               strstr(AudioSystem::getParameters(
                          0,
                          String8("AURISYS_SET_PARAM_RESULT")).string(), "=") + 1);
#endif
    }
}


static void runGetParam(const char *cmd)
{
    if (strncmp(cmd, key_aurisys, sizeof(key_aurisys) - 1) == 0) {
        printf("%s\n",
               strstr(AudioSystem::getParameters(
                          0,
                          String8(cmd)).string(), "=") + 1);
    }
    else {
        printf("%s\n", AudioSystem::getParameters(0, String8(cmd)).string());
    }
}



