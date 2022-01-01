#include <pthread.h>
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

PSP_MODULE_INFO("Pthreads Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

typedef struct
{
    pthread_mutex_t mutex;
    pthread_t owner;
    uint32_t count;
} PthreadMutex;


void *__PHYSFS_platformGetThreadID(void)
{
    return ( (void *) ((size_t) pthread_self()) );
} /* __PHYSFS_platformGetThreadID */


void *__PHYSFS_platformCreateMutex(void)
{
    int rc;
    PthreadMutex *m = (PthreadMutex *) malloc(sizeof (PthreadMutex));
    rc = pthread_mutex_init(&m->mutex, NULL);
    if (rc != 0)
    {
        free(m);
        return NULL;
    } /* if */

    m->count = 0;
    m->owner = (pthread_t) 0xDEADBEEF;
    return ((void *) m);
} /* __PHYSFS_platformCreateMutex */


void __PHYSFS_platformDestroyMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;

    /* Destroying a locked mutex is a bug, but we'll try to be helpful. */
    if ((m->owner == pthread_self()) && (m->count > 0))
        pthread_mutex_unlock(&m->mutex);

    pthread_mutex_destroy(&m->mutex);
    free(m);
} /* __PHYSFS_platformDestroyMutex */


int __PHYSFS_platformGrabMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;
    pthread_t tid = pthread_self();
    if (m->owner != tid)
    {
        if (pthread_mutex_lock(&m->mutex) != 0)
            return 0;
        m->owner = tid;
    } /* if */

    m->count++;
    return 1;
} /* __PHYSFS_platformGrabMutex */

void __PHYSFS_platformReleaseMutex(void *mutex)
{
    PthreadMutex *m = (PthreadMutex *) mutex;
    assert(m->owner == pthread_self());  /* catch programming errors. */
    assert(m->count > 0);  /* catch programming errors. */
    if (m->owner == pthread_self())
        {
            if (--m->count == 0)
                {
                    m->owner = (pthread_t) 0xDEADBEEF;
                    pthread_mutex_unlock(&m->mutex);
                } /* if */
        } /* if */
} /* __PHYSFS_platformReleaseMutex */

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
    void* mutex = __PHYSFS_platformCreateMutex();

    int i = 0;
    while(running())
    {
        pspDebugScreenSetXY(0,0);
        pspDebugScreenPrintf("loop: %d\n",(int)i);
        pspDebugScreenPrintf("Self: %p", __PHYSFS_platformGetThreadID());
        // __PHYSFS_platformGrabMutex(mutex);
        // __PHYSFS_platformGrabMutex(mutex);

        
        // __PHYSFS_platformReleaseMutex(mutex);
        // __PHYSFS_platformReleaseMutex(mutex);

        sceDisplayWaitVblankStart();
        i++;
    }

    __PHYSFS_platformDestroyMutex(mutex);
    sceKernelExitGame();
}
