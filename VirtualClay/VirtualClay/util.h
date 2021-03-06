#pragma once


enum jointEnum {TIP,DIP,PIP,MCP};
enum LR {l,r}; //Left or Right
enum fingerEnum {THUMB,INDEX,MIDDLE,RING,PINKY};
enum boneEnum {DISTAL,INTER,PROXI,META};
#define mbstatus(a) mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,a)

#define mbhud(a) mb::Kernel()->Interface()->HUDMessageShow(a)
#define PLUGINDIR   mb::Kernel()->PluginDirectory("VirtualClay")
#define RESOURCESDIR mb::Kernel()->PluginDirectory("VirtualClay")+"/VirtualClayResources/"

namespace mb = mudbox;
QString VectorToQStringLine(mudbox::Vector v);
QString VectorToQString(mudbox::Vector v);
mb::Vector leapVecToMBVec(Leap::Vector v);
Leap::Vector mbVecToLeapVec(mb::Vector v);
mb::Matrix createRotateXMatrix(float b);
mb::Matrix createRotateYMatrix(float b);
mb::Matrix createRotateZMatrix(float b);
mb::Matrix createTranslation(float x,float y, float z);
mb::Vector RotateVectorAroundPivot(mb::Vector &currPos,mb::Vector &pivot, mb::Vector &rotation);
mb::Vector ScreenSpaceToPixels(mb::Vector);
mb::Vector AngleBetweenTwoPoints(mb::Vector p1, mb::Vector p2);
mb::Vector findDisplacementUV(mb::Base &base, mb::Vector &p0, mb::Vector &p1);
void LeapSleep(unsigned int ms);
__inline mb::Matrix matrixFromVectors(mb::Vector v1,mb::Vector v2,mb::Vector v3);
mb::Matrix AimConstraint(mb::Vector constrained, mb::Vector target);
mb::Vector GetAimRotation(mb::Vector constrained,mb::Vector target);
mb::Vector GetAimRotation(mb::Vector constrained,mb::Vector target);

//https://hci.rwth-aachen.de/materials/publications/jung2014a.pdf