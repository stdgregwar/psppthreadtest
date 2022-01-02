#include <inttypes.h>
#include <stdlib.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspctrl.h>
#include <pspgu.h>
#include <psprtc.h>
#include <cassert>
#include <physfs.h>

PSP_MODULE_INFO("PhysFS sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);


static int exitRequest = 0;

int running()
{
    return !exitRequest;
}

int exitCallback(int arg1, int arg2, void *common)
{
    exitRequest = 1;
    return 0;
}

int callbackThread(SceSize args, void *argp)
{
    int cbid;

    cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

int setupCallbacks(void)
{
    int thid = 0;

    thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        {
            sceKernelStartThread(thid, 0, 0);
        }

    return thid;
}

int main(void)
{
    setupCallbacks();
    pspDebugScreenInit();


    PHYSFS_init(nullptr);
    PHYSFS_addToSearchPath(".",1);

    int i = 0;
    while(running())
    {
        pspDebugScreenSetXY(0,0);
        pspDebugScreenPrintf("loop: %d\n",(int)i);


        sceDisplayWaitVblankStart();
        i++;
    }

    sceKernelExitGame();
}
