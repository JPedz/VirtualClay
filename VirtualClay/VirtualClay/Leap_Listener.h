#pragma once
#include <Leap.h>
#include <LeapMath.h>
using namespace Leap;
class Leap_Listener :public Listener {
public:
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onFrame(const Controller&);
};

