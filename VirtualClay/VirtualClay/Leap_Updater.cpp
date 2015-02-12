#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
  ,frameCounter(0)
{
  hand_l = l;
  hand_r = r;
  leapReader = new Leap_Reader();
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
	mb::Kernel()->Interface()->HUDMessageShow("Frame count has started.", 
    mudbox::Interface::HUDmsgFade);
  frameCounter = 0;
}



void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  if(cEvent == frameEvent) {
    frameCounter++;
    leapReader->updateAll();
    //set finger position and orientation
    
    hand_l->SetPos(leapReader->getPosition_L());
    hand_l->SetRot(leapReader->getDirection_L());
    hand_r->SetPos(leapReader->getPosition_R());
    hand_r->SetRot(leapReader->getDirection_R());
    for(int i = 0 ; i < 5 ; i++) {
      hand_l->SetFingerPos(fingerEnum(i),leapReader->getFingerPosition_L(fingerEnum(i)));
      hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
      hand_r->SetFingerPos(fingerEnum(i),leapReader->getFingerPosition_R(fingerEnum (i)));
      hand_r->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_R(fingerEnum(i)));
    }

  }
}
