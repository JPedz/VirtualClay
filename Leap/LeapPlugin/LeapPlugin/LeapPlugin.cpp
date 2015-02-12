// LeapPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

namespace mb = mudbox;
IMPLEMENT_CLASS (LeapPlugin,mb::Node,"LEAPUI");
MB_PLUGIN( "LeapPlugin", "LeapMotion Plugin", "James Pedlingham", "URL", LeapPlugin::Initializer);

using namespace Leap;

void LeapPlugin::Initializer() {
  mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, "LEAP Initiate",LeapPlugin::Execute); 
}

void LeapPlugin::Execute(void) {
  Leap::Controller controller;
  
  while(!controller.isConnected()) {

  }
  if(controller.isConnected())
    mb::Kernel()->Interface()->SetStatus(mb::Interface::StatusType::stNormal,"Connected to Leap");
//  LeapListener listener;
 // controller.addListener(listener);

}

