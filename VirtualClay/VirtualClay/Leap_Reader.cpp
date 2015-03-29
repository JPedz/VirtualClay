#include "stdafx.h"
#include "Leap_Reader.h"

using namespace Leap;

Leap_Reader::Leap_Reader(void) {
  handvisi.resize(2,false);
  controller.addListener(listener);
  controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
  controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
  controller.config().setFloat("Gesture.ScreenTap.MinForwardVelocity", 5.0);
  controller.config().setFloat("Gesture.ScreenTap.HistorySeconds", 1.0f);
  controller.config().setFloat("Gesture.ScreenTap.MinDistance", 10.0f);
  controller.config().setFloat("Gesture.Swipe.MinLength", 300.0);
  controller.config().setFloat("Gesture.Swipe.MinVelocity", 1000.0);
  controller.config().setFloat("Gesture.Circle.MinRadius", 18.0);
  controller.config().setFloat("Gesture.Circle.MinArc", 3*PI);
  controller.config().save();
  UndoTimeOut = new QTime();
  UndoTimeOut->start();
  QTime *t = new QTime();
  t->start();
  while(!controller.isConnected()) {
    if(t->elapsed() > 5000) {
       mb::Kernel()->Interface()->MessageBox(mb::Interface::msgInformation,
       "Leap Motion Connection",
       "Unable to connect to Leap Motion Controller. Please make sure it is plugged in",
       2,0);
      t->restart();
      break;
    }
    mb::Kernel()->Interface()->SetStatus(mb::Interface::stNormal,"Leap Not Connected");
  }
  if(controller.isConnected()) {
    isConnected =true;
    mbstatus("Leap Connected Successfully");
    mblog("\n Leap Connected Successfully\n");
  } else {
    isConnected =false;
    mblog("\n Leap not Connected\n");
  }
  lastFrameID = controller.frame().id();
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
      HandSetup(f);
      isScreenTap_L = false;
      isScreenTap_R = false;
      isUndo = false;
      isCircleCW_R = false;
      isCircleCCW_R = false;
      isCircleCW_L = false;
      isCircleCCW_L = false;
      Leap::GestureList gestures = f.gestures();
      Leap::SwipeGesture swipeGesture = Leap::Gesture::invalid();
      Leap::CircleGesture circleGesture = Leap::Gesture::invalid();
      Leap::HandList hl;
      std::vector<bool> ext_r = GetExtendedFingers(r);
      std::vector<bool> ext_l = GetExtendedFingers(l);
      for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != f.gestures().end(); gl++)
      {
        hl = (*gl).hands();
        if(hl[0].isLeft()) {
          gestureHand = l;
          //mblog("LeftHand");
        } else {
          gestureHand = r;
          //mblog("RightHand");
        }
        switch ((*gl).type()) {
          case Leap::Gesture::TYPE_CIRCLE:
            circleGesture = CircleGesture(*gl);
            switch((*gl).state()) {
              case Leap::Gesture::STATE_START:
                //Handle starting gestures
                break;
              case Leap::Gesture::STATE_UPDATE:
                //Handle continuing gestures
                break;
              case Leap::Gesture::STATE_STOP:
                if(gestureHand == r) {
                  //mblog("RightHand");
                  if(!ext_r.at(0) && ext_r.at(1) && ext_r.at(2) && !ext_r.at(3) && !ext_r.at(4)) {
                    if(circleGesture.pointable().direction().angleTo(circleGesture.normal()) <= Leap::PI/2) {
                      mblog("circle Radius: "+QString::number(circleGesture.radius())+"\n");
                      isCircleCW_R = true;
                    }
                    if(circleGesture.pointable().direction().angleTo(circleGesture.normal()) >= Leap::PI/2) {
                      mblog("circle Radius: "+QString::number(circleGesture.radius())+"\n");
                      isCircleCCW_R = true;
                    }
                  }
                } else {
                  //mblog("LeftHand");
                    if(!ext_l.at(0) && ext_l.at(1) && ext_l.at(2) && !ext_l.at(3) && !ext_l.at(4)) {
                    if(circleGesture.pointable().direction().angleTo(circleGesture.normal()) <= Leap::PI/2) {
                      mblog("circle Radius: "+QString::number(circleGesture.radius())+"\n");
                      isCircleCW_L = true;
                    }
                    if(circleGesture.pointable().direction().angleTo(circleGesture.normal()) >= Leap::PI/2) {
                      mblog("circle Radius: "+QString::number(circleGesture.radius())+"\n");
                      isCircleCCW_L = true;
                    }
                  }
                }
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
              mbstatus("ScreenTap B");
            case Leap::Gesture::STATE_START:
              //Handle starting gestures
              break;
            case Leap::Gesture::STATE_UPDATE:
              //Handle continuing gestures
              break;
            case Leap::Gesture::STATE_STOP:
              if(gestureHand == l) 
                isScreenTap_L = true;
              else
                isScreenTap_R = true;
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
            if(UndoTimeOut->elapsed() > 500) {
              if(swipeGesture.direction().x < 0 ) {
                if(CheckFingerExtensions(gestureHand,1,1,1,1,1))
                  isUndo = true;
                  UndoTimeOut->restart();
              }
            }
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
      //mblog("Old frame\n");
      return false;
    }
  } else {
    QTime *t = new QTime();
    t->start();
    while(!controller.isConnected()) {
      if(t->elapsed() > 5000) {
         mb::Kernel()->Interface()->MessageBox(mb::Interface::msgInformation,
         "Leap Motion Connection",
         "Unable to connect to Leap Motion Controller. Please make sure it is plugged in",
         2,0);
        break;
      }
      mb::Kernel()->Interface()->SetStatus(mb::Interface::stNormal,"Leap Not Connected");
    }
    handvisi.at(0) = false;
    handvisi.at(1) = false;
    isConnected =false;
    return false;
  }
}

void Leap_Reader::HandSetup(Frame &f) {
  HandList hands = f.hands();
  ToolList tools = f.tools();
  PointableList pointys = f.pointables();
 // mblog("\nHandCount: "+QString::number(hands.count())+"\n");
  //Reconfigure these to ensure that the hand with the greatest confidence chooses
  // which is 'left' and which is right.
  isTool = false;
  if(tools.count() > 0) {
    isTool = true;
    //mblog("IS TOOL\n");
    tool = tools.frontmost();
  }
  isGrabbing_L =false;
  isGrabbing_R =false;
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
    if(isFist(r)) {
      isGrabbing_R =true;
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
        if(isFist(r)) {
          isGrabbing_R =true;
        }
        handvisi.at(0) = false;
        handvisi.at(1) = true;
      }
    } else {
      handvisi.at(0) = false;
      handvisi.at(1) = false;
      ishands = false;
    }
  }
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
  Bone::Type bType = Bone::Type(1);
  f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  b = f.bone(bType);
  directions = LeapDirectionToMudbox(b.direction());
  return directions;
}

mb::Vector Leap_Reader::getFingerDirection_R(fingerEnum fn) {
  Finger f;
  mb::Vector directions;
  Bone b;
  Bone::Type bType = Bone::Type(1);
  f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
  b = f.bone(bType);
  directions = LeapDirectionToMudbox(b.direction());
  return directions;
}


std::vector<mb::Vector> Leap_Reader::getFingerPosition(fingerEnum fn,LR lOrR) {
  Finger f;
  Bone b;
  if(lOrR == l)
    f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  else
    f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
//  mudbox::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,"Finger Pos"+QString::number(fn)+": "+
//      QString::number(f.stabilizedTipPosition().x)+" "+QString::number(f.stabilizedTipPosition().y)+" "+QString::number(f.stabilizedTipPosition().z));

  std::vector<mb::Vector> fingerJoints(4);
  fingerJoints.at(TIP) = scale*(mb::Vector(f.tipPosition().x,f.tipPosition().y,f.tipPosition().z));
  fingerJoints.at(DIP) = scale*(mb::Vector(f.jointPosition(f.JOINT_DIP).x,f.jointPosition(f.JOINT_DIP).y,f.jointPosition(f.JOINT_DIP).z));
  fingerJoints.at(PIP) = scale*(mb::Vector(f.jointPosition(f.JOINT_PIP).x,f.jointPosition(f.JOINT_PIP).y,f.jointPosition(f.JOINT_PIP).z));
  fingerJoints.at(MCP) = scale*(leapVecToMBVec(f.jointPosition(f.JOINT_MCP)));
  //mblog("Finger Pos: "+VectorToQStringLine(fingerJoints.at(0)));
  return fingerJoints;
}

std::vector<mb::Vector> Leap_Reader::getBoneOrients(fingerEnum fn,LR lOrR) {
  Finger f;
  Bone b;
  if(lOrR == l)
    f = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  else
    f = hand_r.fingers().fingerType(Finger::Type(fn))[0];
  std::vector<mb::Vector> bones(4);
  bones.at(DISTAL) = RAD_TO_DEG*(leapVecToMBVec(f.bone(b.TYPE_DISTAL).direction()));
  if(lOrR == l)
    bones.at(DISTAL) = RAD_TO_DEG*(leapVecToMBVec(f.bone(b.TYPE_DISTAL).basis().xBasis));
  bones.at(INTER) = RAD_TO_DEG*(leapVecToMBVec(f.bone(b.TYPE_INTERMEDIATE).basis().yBasis));
  bones.at(PROXI) = RAD_TO_DEG*(leapVecToMBVec(f.bone(b.TYPE_PROXIMAL).basis().yBasis));
  bones.at(META) = RAD_TO_DEG*(leapVecToMBVec(f.bone(b.TYPE_METACARPAL).basis().yBasis));
  return bones;
}

mb::Vector Leap_Reader::getMotionDirection(fingerEnum fn, LR lOrR) {
  mb::Vector dir;
  Leap::Finger fing;
  if(lOrR == l) {
    fing = hand_l.fingers().fingerType(Finger::Type(fn))[0];
  } else {
    fing = hand_r.fingers().fingerType(Finger::Type(fn))[0];
  }
  dir = leapVecToMBVec(fing.tipVelocity());
  dir.Normalize();
  return dir;
}

mb::Vector Leap_Reader::getDirection_L(void) {
  const float yaw = hand_l.direction().yaw();
  const float roll = hand_l.palmNormal().roll();
  const float pitch = hand_l.direction().pitch();
  return mb::Vector(-pitch,yaw,-roll)*RAD_TO_DEG;
}

mb::Vector Leap_Reader::getDirection_R(void) {
 // http://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate
  const float yaw = hand_r.direction().yaw();
  const float roll = hand_r.palmNormal().roll();
  const float pitch = hand_r.direction().pitch();
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
      if(hand_l.grabStrength() > 0.9)
        return true;
      break;
    case(r):
      if(hand_r.grabStrength() > 0.9)
        return true;
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

bool Leap_Reader::CheckRotateHandGesture(LR lOrR) {

  if(lOrR == l) {
    std::vector<bool> ext_l = GetExtendedFingers(l);
    if(ext_l.at(0) && ext_l.at(1) && !ext_l.at(2) && !ext_l.at(3) && ext_l.at(4))
      return true;
  } else {
    std::vector<bool> ext_r = GetExtendedFingers(r);
    if(ext_r.at(0) && ext_r.at(1) && !ext_r.at(2) && !ext_r.at(3) && ext_r.at(4))
      return true;
  }
  return false;
}


bool Leap_Reader::CheckScaleHandGesture(LR lOrR) {

  if(lOrR == l) {
    std::vector<bool> ext_l = GetExtendedFingers(l);
    if(!ext_l.at(0) && ext_l.at(1) && !ext_l.at(2) && !ext_l.at(3) && ext_l.at(4))
      return true;
  } else {
    std::vector<bool> ext_r = GetExtendedFingers(r);
    if(!ext_r.at(0) && ext_r.at(1) && !ext_r.at(2) && !ext_r.at(3) && ext_r.at(4))
      return true;
  }
  return false;
}


mb::Vector Leap_Reader::rotateScene() {
  //TODO: Do I need this?
  std::vector<bool> ext_l = GetExtendedFingers(l);
  std::vector<bool> ext_r = GetExtendedFingers(r);

  if(handvisi.at(l)) {
    //Using Left Hand;
    if(ext_l.at(0) && ext_l.at(1) && !ext_l.at(2) && !ext_l.at(3) && ext_l.at(4)) {
      if(hand_l.rotationProbability(controller.frame(10)) > 0.6) {
        float ang = hand_l.rotationAngle(controller.frame(10));
        Leap::Vector rotAxis = hand_l.rotationAxis(controller.frame(10));
        //return ang*mb::Vector(0, rotAxis.y, 0);
        return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
      } else {
        mblog("Left Hand Rotate Probability = "+QString::number(hand_l.rotationProbability(controller.frame(10))));
      }
    }
  } else {
    if(ext_r.at(0) && ext_r.at(1) && !ext_r.at(2) && !ext_r.at(3) && ext_r.at(4)) {
      //Using Right Hand;
      if(hand_r.rotationProbability(controller.frame(10)) > 0.6) {
        float ang = hand_r.rotationAngle(controller.frame(10));
        Leap::Vector rotAxis = hand_r.rotationAxis(controller.frame(10));
        //return ang*mb::Vector(0, rotAxis.y, 0);
        return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
      } else {
        mblog("Right Hand Rotate Probability = "+QString::number(hand_r.rotationProbability(controller.frame(10))));
      }
    }
  }
  return mb::Vector(0,0,0);

  //TODO:Smooth
  //if(isFist(l) && isFist(r)) {
  //  return mb::Vector(0,0,0);
  //} else {
  //  if(isFist(l)) {
  //    mblog("Left is Fist\n");
  //     float ang = hand_r.rotationAngle(controller.frame(1));
  //     Leap::Vector rotAxis = hand_r.rotationAxis(controller.frame(1));
  //     //return ang*mb::Vector(0, rotAxis.y, 0);
  //     return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
  //  } else if(isFist(r)){
  //    mblog("Right is Fist\n");
  //     float ang = hand_l.rotationAngle(controller.frame(1));
  //     Leap::Vector rotAxis = hand_l.rotationAxis(controller.frame(1));
  //     //return ang*mb::Vector(0, rotAxis.y, 0);
  //     return ang*mb::Vector(rotAxis.x, rotAxis.y, rotAxis.z);
  //    //Inverses the left hand stuff
  //  } else {
  //    //no fist no rotation;
  //    return mb::Vector(0,0,0);
  //  }
  //}
}

bool Leap_Reader::isVisible(LR lr) {
  return handvisi.at(lr);
}

std::vector<mb::Vector> Leap_Reader::GetToolPositions() {
  std::vector<mb::Vector> toolLocs;
  if(isTool) {
    toolLocs.push_back(leapVecToMBVec(tool.stabilizedTipPosition()));
    toolLocs.push_back(leapVecToMBVec(
      tool.stabilizedTipPosition()-(tool.direction()*tool.length())
    ));
  }
  return toolLocs;
}


mb::Vector Leap_Reader::GetToolDirection() {
  mb::Vector tooldir;
  if(isTool) {
    tooldir = (leapVecToMBVec(tool.direction()))*RAD_TO_DEG;
  }
  return tooldir;
}

mb::Vector Leap_Reader::getToolMotionDirection() {
  mb::Vector dir;
  dir = leapVecToMBVec(tool.tipVelocity());
  dir.Normalize();
  return dir;
}

bool Leap_Reader::CheckFingerExtensions(LR lOrR,bool ext0,bool ext1,bool ext2,bool ext3,bool ext4) {
  std::vector<bool> b;
  if(lOrR == l) {
    b = GetExtendedFingers(l);
  } else {
    b = GetExtendedFingers(r);
  }
  if((b.at(0) == ext0) &&
    (b.at(1) == ext1) &&
    (b.at(2) == ext2) &&
    (b.at(3) == ext3) &&
    (b.at(4) == ext4))
      return true;
  else
    return false;

}
