#include "stdafx.h"
#include "Leap_Listener.h"

using namespace Leap;


void Leap_Listener::onConnect(const Controller& controller) {
  mudbox::Kernel()->Log("\nLeap Connected\n");
}
void Leap_Listener::onDisconnect(const Controller& controller) {
  mudbox::Kernel()->Log("\nLeap Disconnected\n");
  mudbox::Kernel()->Interface()->MessageBox(mudbox::Interface::msgInformation,
  "Leap Motion Connection", 
  "Connection Lost to Leap Motion Controller. Please make sure it is plugged in",
  2,0);
}

void Leap_Listener::onFrame(const Controller& controller) {
  //Forces redraw of frame to trigger update on screen
  mudbox::Kernel()->ViewPort()->Redraw();
  //mblog("RedrawFrame");
}
