#include "stdafx.h"
#include "Leap_Reader.h"

using namespace Leap;

Leap_Reader::Leap_Reader(void) {
  handvisi.resize(2,false);
  controller.addListener(listener); 
  QTime *t = new QTime();
  t->start();
  while(!controller.isConnected()) {
    if(t->elapsed() > 5000)
       mb::Kernel()->Interface()->MessageBox(mb::Interface::msgInformation,
       "Leap Motion Connection", 
       "Unable to connect to Leap Motion Controller. Please make sure it is plugged in",
       2,0);
      break;
    mb::Kernel()->Interface()->SetStatus(mb::Interface::stNormal,"Leap Not Connected");
  }
  if(controller.isConnected()) {
    isConnected =true;
    mb::Kernel()->Interface()->SetStatus(mb::Interface::stNormal,"Leap Connected Successfully");
    mblog("\n Leap Connected Successfully\n");
  } else {
    isConnected =false;
    mblog("\n Leap not Connected\n");
  }
}

Leap_Reader::~Leap_Reader(void)
{
  controller.removeListener(listener);
}

void Leap_Reader::updateAll(void) {
  if(controller.isConnected()) {
    isConnected =true;
    Frame f = controller.frame();
    updateDirection(f);
  } else {
    handvisi.at(0) = false;
    handvisi.at(1) = false;
    isConnected =false;
  }
}

void Leap_Reader::HandSetup(Frame &f) {
  HandList hands = f.hands();
 // mblog("\nHandCount: "+QString::number(hands.count())+"\n");
  //Reconfigure these to ensure that the hand with the greatest confidence chooses
  // which is 'left' and which is right.
  
  ishands = true;
  if(hands.count() > 1) {
    if(hands.leftmost().confidence() >= hands.rightmost().confidence()) {
      if(hands.leftmost().isLeft()) {
        hand_l = hands.leftmost();
        hand_r = hands.rightmost();
      } else {
        hand_r = hands.leftmost();
        hand_l = hands.rightmost();
      }
    } else {
      if(hands.rightmost().isRight()) {
          hand_l = hands.leftmost();
          hand_r = hands.rightmost();
        } else {
          hand_r = hands.leftmost();
          hand_l = hands.rightmost();
        }
    }
    handvisi.at(0) = true;
    handvisi.at(1) = true;
  } else {
    if(hands.count() == 1) {
      if(hands.leftmost().isLeft()) {
        hand_l = hands.leftmost();
        handvisi.at(0) = true;
        handvisi.at(1) = false;
      } else {
        hand_r = hands.leftmost();
        handvisi.at(0) = false;
        handvisi.at(1) = true;
      }
    } else {
      handvisi.at(0) = false;
      handvisi.at(1) = false;
      ishands = false;
    }
  }/*
  if(hands.leftmost().isLeft()) {
    hand_l = hands.leftmost();
  } else if(hands.rightmost().isLeft()) {
    hand_l = hands.rightmost();
  }
  if(hands.leftmost().isRight()) {
    hand_r = hands.leftmost();
  } else if(hands.rightmost().isRight()) {
    hand_r = hands.rightmost();
  } else {
    hand_r = hands.rightmost();
    hand_l = hands.leftmost();
  }*/
}

mb::Vector Leap_Reader::LeapDirectionToMudbox(Leap::Vector dir) {
  return mb::Vector(dir.yaw(),dir.pitch(),dir.roll())*RAD_TO_DEG;
}

mb::Vector Leap_Reader::LeapPositionToMudbox(Leap::Vector dir) {
  return mb::Vector(dir.x,dir.y,dir.z);
}

mb::Vector Leap_Reader::getFingerDirection_L(fingerEnum fn) {
  Finger f;
  mb::Vector directions;
  Bone b;
  f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  b = f.bone(Bone::Type::TYPE_PROXIMAL);
  directions = LeapDirectionToMudbox(b.direction());
  return directions;
}

mb::Vector Leap_Reader::getFingerDirection_R(fingerEnum fn) {
  Finger f;
  mb::Vector directions;
  Bone b;
  f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
  b = f.bone(Bone::Type::TYPE_PROXIMAL);
  directions = LeapDirectionToMudbox(b.direction());
  return directions;
}


mb::Vector Leap_Reader::getFingerPosition_L(fingerEnum fn) {
  Finger f;
  Bone b;
  f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
//  mudbox::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,"Finger Pos"+QString::number(fn)+": "+
//      QString::number(f.stabilizedTipPosition().x)+" "+QString::number(f.stabilizedTipPosition().y)+" "+QString::number(f.stabilizedTipPosition().z));  
  return mb::Vector(f.tipPosition().x,f.tipPosition().y,f.tipPosition().z);
}

mb::Vector Leap_Reader::getFingerPosition_R(fingerEnum fn) {
  Finger f;
  Bone b;
  f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
//  mudbox::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,"Finger Pos"+QString::number(fn)+": "+
//      QString::number(f.stabilizedTipPosition().x)+" "+QString::number(f.stabilizedTipPosition().y)+" "+QString::number(f.stabilizedTipPosition().z));  
  return mb::Vector(f.tipPosition().x,f.tipPosition().y,f.tipPosition().z);
}

mb::Vector Leap_Reader::getDirection_L(void) {
  return mb::Vector(hand_l.direction().yaw(),hand_l.direction().pitch(),hand_l.direction().roll()-PI)*RAD_TO_DEG;
}

mb::Vector Leap_Reader::getDirection_R(void) {
 // http://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate
  float x,y,z;
  float yaw,roll,pitch;
  yaw = hand_r.palmNormal().yaw();
  roll = hand_r.palmNormal().roll();
  pitch = hand_r.palmNormal().pitch();
  x = cos(yaw)*cos(pitch);
  y = sin(yaw)*cos(pitch);
  z = sin(pitch);
  return mb::Vector(yaw,pitch,roll)*RAD_TO_DEG;
}

mb::Vector Leap_Reader::getPosition_L(void) {
  return mb::Vector(hand_l.palmPosition().x,hand_l.palmPosition().y,hand_l.palmPosition().z);
}

mb::Vector Leap_Reader::getPosition_R(void) {
  return mb::Vector(hand_r.palmPosition().x,hand_r.palmPosition().y,hand_r.palmPosition().z);
}

bool Leap_Reader::isFist(LR lr) {
  Frame f;
  switch(lr) {
    case(l):
      if(hand_l.grabStrength() > 0.9) {
        return true;
      }
      break;
    case(r):
      if(hand_r.grabStrength() > 0.9) {
        return true;
      }
      break;
  }
  return false;
}


mb::Vector Leap_Reader::rotateScene() {
  //TODO:Smooth
  if(isFist(l) && isFist(r)) {
    return mb::Vector(0,0,0);
  } else {
    if(isFist(l)) {
      mblog("Left is Fist\n");
       float ang = hand_r.rotationAngle(controller.frame(1));
       Leap::Vector rotAxis = hand_r.rotationAxis(controller.frame(1));
       //return ang*mb::Vector(0, rotAxis.y, 0);
       return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
    } else if(isFist(r)){
      mblog("Right is Fist\n");
       float ang = hand_l.rotationAngle(controller.frame(1));
       Leap::Vector rotAxis = hand_l.rotationAxis(controller.frame(1));
       //return ang*mb::Vector(0, rotAxis.y, 0);
       return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
      //Inverses the left hand stuff
    } else {
      //no fist no rotation;
      return mb::Vector(0,0,0);
    }
  }
}

bool Leap_Reader::isVisible(LR lr) {
  return handvisi.at(lr);
}

void Leap_Reader::updateDirection(Frame &f) {
  HandSetup(f);
}


