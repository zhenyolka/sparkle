#include "platform_na.h"
#include "were/were_timer.h"
#include <stdexcept>

//==================================================================================================

class PointerState
{
public:
    PointerState() : down(false), x(0), y(0) {}
    bool down;
    int x;
    int y;
};

class PlatformNA : public Platform
{
public:
    ~PlatformNA();
    PlatformNA(WereEventLoop *loop);

    int start();
    int stop();

private:
    void timeout();

private:
    WereEventLoop *_loop;

    WereTimer *_timer;

    bool _draw;
};

PlatformNA::~PlatformNA()
{
    delete _timer;
}

PlatformNA::PlatformNA(WereEventLoop *loop)
{
    _loop = loop;

    _draw = true;

    _timer = new WereTimer(_loop);
    _timer->timeout.connect(WereSimpleQueuer(loop, &PlatformNA::timeout, this));
}

int PlatformNA::start()
{
    _timer->start(1000/60, false);

    return 0;
}

int PlatformNA::stop()
{
    return 0;
}

void PlatformNA::timeout()
{
    if (_draw)
        draw();
}

Platform *platform_na_create(WereEventLoop *loop)
{
    return new PlatformNA(loop);
}

//==================================================================================================

