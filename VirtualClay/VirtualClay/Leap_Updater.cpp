#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
{
  idList = idl;
  hand_l = l;
  hand_r = r;
  leapReader = new Leap_Reader();
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
	mb::Kernel()->Interface()->HUDMessageShow("Frame count has started.", 
    mudbox::Interface::HUDmsgFade);
}

mb::Vector Leap_Updater::fitToCameraSpace() {
  int camID = idList->getViewCam();
  mblog(QString::number(camID));
  cameraWrapper *viewCam = new cameraWrapper(camID);
  mb::Vector camPos = viewCam->getPosision();
  mb::Vector pO = mb::Vector(0,250,600);
  mb::Vector heightOffset = mb::Vector(0,100,0);
  mb::Vector camForward = viewCam->getForward();
  int forwardfactor = (int)(pO.Length());
  mb::Vector centrePoint = camPos - (camForward*forwardfactor) - heightOffset;
  return centrePoint;
}


void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    leapReader->updateAll();
    mb::Vector cameraPivot = fitToCameraSpace();
    hand_l->SetVisi(leapReader->isVisible(l));
    hand_r->SetVisi(leapReader->isVisible(r));
    //set finger position and orientation
    hand_l->SetPos(cameraPivot + leapReader->getPosition_L());
    hand_l->SetRot(leapReader->getDirection_L());
    hand_r->SetPos(cameraPivot + leapReader->getPosition_R());
    hand_r->SetRot(leapReader->getDirection_R());
    for(int i = 0 ; i < 5 ; i++) {
      hand_l->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_L(fingerEnum(i)));
      hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
      hand_r->SetFingerPos(fingerEnum(i),cameraPivot + leapReader->getFingerPosition_R(fingerEnum (i)));
      hand_r->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_R(fingerEnum(i)));
      hand_l->RotateAroundPivot(fingerEnum(i),mb::Vector(0,180,0),cameraPivot);
      hand_r->RotateAroundPivot(fingerEnum(i),mb::Vector(0,180,0),cameraPivot);
    }
  }
}
