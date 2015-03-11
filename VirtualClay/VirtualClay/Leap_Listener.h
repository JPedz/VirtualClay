#pragma once
#include <Leap.h>
#include <LeapMath.h>
using namespace Leap;
class Leap_Listener :public Listener {
  bool clearHandsFrame;//Render 1 extra frames from when the hands go missing to clear scene
public:
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onFrame(const Controller&);
};

