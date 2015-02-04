// VirtualClay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cameraWrapper.h"
#include "menuUI.h"
IMPLEMENT_CLASS (menuUI,mb::Node,"menuUI");
namespace mb = mudbox;
MB_PLUGIN( "VirtualClay", "CameraCreator", "James Pedlingham", "URL", VirtualClay::Initializer);

void VirtualClay::Initializer(void) {
  //mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, QObject::tr("TurnOn"), VirtualClay::Execute);
  //mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, QObject::tr("TurnOff"),VirtualClay::Cleanup);
  mb::Kernel()->Interface()->AddClassMenuItem(mb::Interface::menuPlugins,"Leap Motion",menuUI::StaticClass(),QObject::tr("Initialise"));
}


void VirtualClay::Cleanup(void) {
    mb::Node *n = mb::Node::ByID(860);
    QString s = n->ID();
	  mb::Kernel()->Interface()->HUDMessageShow(s);
    n->~Node();
}

void VirtualClay::Execute(void) {
  cameraWrapper *L_Cam = new cameraWrapper("L_HandCam");
  cameraWrapper *R_Cam = new cameraWrapper("R_HandCam");
  L_Cam->addCameraToScene();
  R_Cam->addCameraToScene();
  QString s = QString::number(L_Cam->getID());
  
	mb::Kernel()->Interface()->HUDMessageShow(s);
  
  mb::Node *n = mb::Node::ByID(L_Cam->getID());
  mb::Kernel()->Interface()->HUDMessageShow(n->DisplayName());

  mb::Transformation *nd = mb::CreateInstance<mb::Transformation>();
  nd->SetPosition(mb::Vector(0.0,0.0,0.0));
  nd->SetDisplayName("NEWNODE");
  
  L_Cam->setAim(mb::Vector(0,0,0));
  R_Cam->setAim(mb::Vector(0,0,0));
  L_Cam->setTNode();
  R_Cam->setTNode();
  L_Cam->setTranslation(mb::Vector(200.0f,200.0f,500.0f));
  R_Cam->setTranslation(mb::Vector(-200.0f,200.0f,500.0f));

  
	mb::Kernel()->ViewPort()->Redraw();
}