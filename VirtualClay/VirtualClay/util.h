#pragma once
#include "stdafx.h"
enum jointEnum {TIP,DIP,PIP};
enum LR {l,r}; //Left or Right
enum fingerEnum {THUMB,INDEX,MIDDLE,RING,PINKY};
#define mbstatus(a) mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,a)



namespace mb = mudbox;
QString VectorToQStringLine(mudbox::Vector v);
QString VectorToQString(mudbox::Vector v);
mb::Matrix createRotateXMatrix(float b);
mb::Matrix createRotateYMatrix(float b);
mb::Matrix createRotateZMatrix(float b);
mb::Matrix createTranslation(float x,float y, float z);
mb::Vector RotateVectorAroundPivot(mb::Vector currPos,mb::Vector pivot, mb::Vector rotation);

//https://hci.rwth-aachen.de/materials/publications/jung2014a.pdf