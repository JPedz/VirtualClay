#include "stdafx.h"
#include "Leap_Reader.h"

using namespace Leap;

Leap_Reader::Leap_Reader(void) {
  handvisi.resize(2,false);
  controller.addListener(listener); 
  controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
  controller.config().setFloat("Gesture.Swipe.MinLength", 300.0);
  controller.config().setFloat("Gesture.Swipe.MinVelocity", 1000.0);
  //controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
  QTime *t = new QTime();
  t->start();
  lastFrameID = controller.frame().id();
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

void Leap_Reader::SetScale(mb::Vector s) {
  scale = s;
}

bool Leap_Reader::updateAll(void) {
  if(controller.isConnected()) {
    isConnected =true;
    Frame f = controller.frame();
    if(f.id() != lastFrameID) {
      updateDirection(f);
      isScreenTap = false;
      isUndo = false;
      isCircleCW = false;
      Leap::GestureList gestures = f.gestures();
      Leap::SwipeGesture swipeGesture = Leap::Gesture::invalid();
      Leap::CircleGesture circleGesture = Leap::Gesture::invalid();
      for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != f.gestures().end(); gl++)
      {
         switch ((*gl).type()) {
           case Leap::Gesture::TYPE_CIRCLE:
             //http://doc.qt.io/qt-5/qtopengl-2dpainting-example.html
              switch((*gl).state()) {
                case Leap::Gesture::STATE_START:
                  //Handle starting gestures
                  break;
                case Leap::Gesture::STATE_UPDATE:
                  //Handle continuing gestures
                  break;
                case Leap::Gesture::STATE_STOP:
                  circleGesture = CircleGesture(*gl);
                  if(circleGesture.pointable().direction().angleTo(circleGesture.normal()) <= Leap::PI/2)
                    isCircleCW = true;
                  //Handle swipe gestures
                  break;
                default:
                  break;
              }
              break;
            case Leap::Gesture::TYPE_KEY_TAP:
              //Handle key tap gestures
              break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
              switch ((*gl).state()) {
                case Leap::Gesture::STATE_START:
                  //Handle starting gestures
                  break;
                case Leap::Gesture::STATE_UPDATE:
                  //Handle continuing gestures
                  break;
                case Leap::Gesture::STATE_STOP:
                  isScreenTap = true;
                  //Handle ending gestures
                  break;
                default:
                  //Handle unrecognized states
                  break;
              }
              //Handle screen tap gestures
              break;
          case Leap::Gesture::TYPE_SWIPE:
              swipeGesture = SwipeGesture(*gl);
              if(swipeGesture.direction().x < 0 )
                isUndo = true;
              //Handle swipe gestures
              break;
          default:
              //Handle unrecognized gestures
              break;
        }
      }
      lastFrameID = f.id();
      return true;
    } else {
      mblog("Old frame\n");
      return false;
    }
  } else {
    handvisi.at(0) = false;
    handvisi.at(1) = false;
    isConnected =false;
    return false;
  }
}

void Leap_Reader::HandSetup(Frame &f) {
  HandList hands = f.hands();
 // mblog("\nHandCount: "+QString::number(hands.count())+"\n");
  //Reconfigure these to ensure that the hand with the greatest confidence chooses
  // which is 'left' and which is right.
  
  isGrabbing_L =false;
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
    if(isFist(l)) {
      isGrabbing_L =true;
    }
  } else {
    if(hands.count() == 1) {
      if(hands.leftmost().isLeft()) {
        hand_l = hands.leftmost();
        handvisi.at(0) = true;
        handvisi.at(1) = false;
        if(isFist(l)) {
          isGrabbing_L =true;
        }
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


std::vector<mb::Vector> Leap_Reader::getFingerPosition(fingerEnum fn,LR LOrR) {
  Finger f;
  Bone b;
  if(LOrR == l)
    f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  else
    f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
//  mudbox::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,"Finger Pos"+QString::number(fn)+": "+
//      QString::number(f.stabilizedTipPosition().x)+" "+QString::number(f.stabilizedTipPosition().y)+" "+QString::number(f.stabilizedTipPosition().z));  
  
  std::vector<mb::Vector> fingerJoints(3);
  fingerJoints.at(TIP) = scale*(mb::Vector(f.tipPosition().x,f.tipPosition().y,f.tipPosition().z));
  fingerJoints.at(DIP) = scale*(mb::Vector(f.jointPosition(f.JOINT_DIP).x,f.jointPosition(f.JOINT_DIP).y,f.jointPosition(f.JOINT_DIP).z));
  fingerJoints.at(PIP) = scale*(mb::Vector(f.jointPosition(f.JOINT_PIP).x,f.jointPosition(f.JOINT_PIP).y,f.jointPosition(f.JOINT_PIP).z));
  //mblog("Finger Pos: "+VectorToQStringLine(fingerJoints.at(0)));
  return fingerJoints;
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
  float yaw,roll,pitch;
  yaw = hand_l.direction().yaw();
  roll = hand_l.palmNormal().roll();
  pitch = hand_l.direction().pitch();
  return mb::Vector(-pitch,yaw,-roll)*RAD_TO_DEG;
  //return mb::Vector(hand_l.direction().yaw(),hand_l.direction().pitch(),hand_l.palmNormal().roll()-PI)*RAD_TO_DEG;
}

mb::Vector Leap_Reader::getDirection_R(void) {
 // http://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate
  float yaw,roll,pitch;
  yaw = hand_r.direction().yaw();
  roll = hand_r.palmNormal().roll();
  pitch = hand_r.direction().pitch();
  return mb::Vector(-pitch,yaw,-roll)*RAD_TO_DEG;
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

std::vector<bool> Leap_Reader::GetExtendedFingers(LR lOrR) {
  std::vector<bool> extendedList(5);
  if(lOrR == l) {
    for(int i = 0 ; i < 5 ; i++) {
      extendedList.at(i) = hand_l.fingers().fingerType(Finger::Type(i))[0].isExtended();
    }
  } else {
    for(int i = 0 ; i < 5 ; i++) {
      extendedList.at(i) = hand_r.fingers().fingerType(Finger::Type(i))[0].isExtended();
    }
  }
  return extendedList;
}


mb::Vector Leap_Reader::TestFunct() {
  Leap::Vector wristPoint = hand_l.wristPosition();
  Leap::Vector tripoint1 = hand_l.fingers().leftmost().jointPosition(Finger::JOINT_MCP);
  Leap::Vector tripoint2 = hand_l.fingers().rightmost().jointPosition(Finger::JOINT_MCP);
  return mb::Vector(0,0,0);
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


