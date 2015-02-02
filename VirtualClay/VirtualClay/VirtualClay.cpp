// VirtualClay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cameraWrapper.h"

namespace mb = mudbox;

MB_PLUGIN( "VirtualClay", "CameraCreator", "James Pedlingham", "URL", VirtualClay::Initializer);

void VirtualClay::Initializer(void) {
  mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, QObject::tr("CreateCam"), VirtualClay::Execute);
}




void VirtualClay::Execute(void) {
  cameraWrapper *L_Cam = new cameraWrapper("L_HandCam");
  cameraWrapper *R_Cam = new cameraWrapper("R_HandCam");
  L_Cam->addCameraToScene();
  R_Cam->addCameraToScene();
  
  L_Cam->setAim(mb::Vector(0,0,0));
  R_Cam->setAim(mb::Vector(0,0,0));
  L_Cam->setTNode();
  R_Cam->setTNode();
  L_Cam->setTranslation(mb::Vector(200.0f,200.0f,500.0f));
  R_Cam->setTranslation(mb::Vector(-200.0f,200.0f,500.0f));

	mb::Kernel()->ViewPort()->Redraw();
}