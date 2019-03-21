#ifndef PLATFORM_H
#define PLATFORM_H

#include "were/were_function.h"
#include "were/were_signal.h"
#include <EGL/egl.h>

class Platform
{
public:
    virtual ~Platform() {}
    virtual int start() = 0;
    virtual int stop() = 0;

    WereFunction<int ()> getVID;
    WereSignal<void (NativeDisplayType)> initializeForNativeDisplay;
    WereSignal<void (NativeWindowType)> initializeForNativeWindow;

    WereSignal<void ()> draw;
};

#endif /* PLATFORM_H */
