#include "stdafx.h"
#include "Leap_Tool.h"
#include "QtGui/qimage.h"
#include "QtGui/qrgb.h"

Leap_Tool::Leap_Tool(void)
{
  tools.resize(2);
  tools.at(0) = new Leap_Fingers("ToolTIP");
  tools.at(1) = new Leap_Fingers("ToolBase");
  tools.at(0)->ImportGeo();
  tools.at(1)->ImportGeo();
  tools.at(0)->SetScale(mb::Vector(0.1f,0.1f,0.1f));
  tools.at(1)->SetScale(mb::Vector(0.1f,0.1f,0.1f));
  img = new QImage();
  OriginalImg = new QImage(RESOURCESDIR+"stamp1.png");
  *img = OriginalImg->copy();
  img->mirrored();
  mblog("luminance at 0,0 = "+QString::number(qGray(OriginalImg->pixel(0.0f,0.0f)))+"\n");
  mblog("luminance at 100,100 = "+QString::number(qGray(OriginalImg->pixel(100.0f,100.0f)))+"\n");
  mblog("luminance at 0,0 = "+QString::number(qGray(img->pixel(0.0f,0.0f)))+"\n");
  mblog("luminance at 100,100 = "+QString::number(qGray(img->pixel(100,100)))+"\n");
  //stamp = mb::CreateInstance<mb::Image>();
  //stamp->Load(RESOURCESDIR+"stamp1.png");
  stamp = mb::CreateInstance<mb::Image>();
  stamp->ConvertFromQImage(*img);
  mblog("luminance at 0,0 = "+QString::number(stamp->ColorAt(0.0f,0.0f).Luminance())+"\n");
  mblog("luminance at 100,100 = "+QString::number(stamp->ColorAt(100.0f,100.0f).Luminance())+"\n");

  server = new AirPen_Server();
  mblog("Started server!\n");
}

Leap_Tool::Leap_Tool(Leap_Hand *copyNode)
{
  tools.resize(2);
  tools.at(0) = new Leap_Fingers("ToolTIP");
  tools.at(1) = new Leap_Fingers("ToolBase");
  tools.at(0)->CopyGeo(copyNode->GetPalm());
  tools.at(1)->CopyGeo(copyNode->GetPalm());
  tools.at(0)->SetScale(mb::Vector(0.03f,0.03f,0.03f));
  tools.at(1)->SetScale(mb::Vector(0.07f,0.07f,0.07f));
  img = new QImage();
  OriginalImg = new QImage(RESOURCESDIR+"stamp1.png");
  *img = OriginalImg->copy();
  img->mirrored();
  stamp = mb::CreateInstance<mb::Image>();
  stamp->ConvertFromQImage(*img);
  server = new AirPen_Server();
  mblog("Started server!\n");
}


void Leap_Tool::SendToServer(int stage) {
  server->SendMsg(stage);
}


float Leap_Tool::GetStampStrength(mb::Vector &uv) {
  unsigned int u = stamp->Width()*0.5 + uv.x;
  unsigned int v = stamp->Height()*0.5 + uv.z;
  if(u < 0) {
    u = 0;
  } else if(u >= stamp->Width()) {
    u = stamp->Width()-1;
  }
  if(v < 0) {
    v = 0;
  } else if(v >= stamp->Height()) {
    v = stamp->Height()-1;
  }
  mblog("Getting stamp co-ordinates "+QString::number(u)+" "+QString::number(v)+" = "+QString::number(stamp->ColorAt(u,v).Luminance())+"\n");
  return stamp->ColorAt(u,v).Luminance();
}


Leap_Tool::~Leap_Tool(void)
{

}


void Leap_Tool::SetStamp(QString fullPath) {
  if(stamp == NULL || img == NULL) { 
    OriginalImg = new QImage(fullPath);
    img = new QImage();
    *img = OriginalImg->copy();
    stamp = mb::CreateInstance<mb::Image>();
  }
  stamp->ConvertFromQImage(*img);
}

void Leap_Tool::ReleaseStamp() {
  delete img;
  img = NULL;
  delete stamp;
  stamp = NULL;
}

void Leap_Tool::ResizeStamp(float x, float y) {
  if(img != NULL) {
    if(img->width() != x && img->height() != y) {
      *img = OriginalImg->copy();
      img = new QImage(img->scaled(x,y));
      mblog("NEW IMG HEIGHT"+QString::number(img->height())+"\n");
      stamp->ConvertFromQImage(*img);
    } else {
      mblog("OLD IMG HEIGHT"+QString::number(img->height())+"\n");
    }
  } else {
    mblog("img null\n");
  }
}

mb::Image* Leap_Tool::GetStamp() {
  return stamp;
}

void Leap_Tool::SetVisi(bool vis) {
  tools.at(0)->SetVisi(vis);
  tools.at(1)->SetVisi(vis);
}

void Leap_Tool::SetPos(int i, mb::Vector &pos) {
  if(i < 2)
    tools.at(i)->SetPos(pos);
  else 
    mblog("Tool Position Index out of Range\n");
}

void Leap_Tool::SetPos(std::vector<mb::Vector> &pos) {
  if( pos.size() != 2) {
    mblog("Tool pos vector size incorrect\n");
  } else {
    tools.at(0)->SetPos(pos.at(0));
    tools.at(1)->SetPos(pos.at(1));
  }
}



void Leap_Tool::SetRot(int i, mb::Vector &pos) {
  if(i < 2)
    tools.at(i)->SetRot(pos);
  else 
    mblog("Tool Position Index out of Range\n");
}



void Leap_Tool::SetRot(mb::Vector &rotation) {
    tools.at(0)->SetRotMatrix(rotation);
    tools.at(1)->SetRotMatrix(rotation);
}

void Leap_Tool::RotateAroundPivot(int i, mb::Vector &a, mb::Vector &pivot) {
  if(i < 2)
    tools.at(i)->RotateAroundPivot(a,pivot);
  else 
    mblog("Tool Position Index out of Range\n");
}

void Leap_Tool::RotateAroundPivot(mb::Vector &a, mb::Vector &pivot) {
  tools.at(0)->RotateAroundPivot(a,pivot);
  tools.at(1)->RotateAroundPivot(a,pivot);
}

mb::Vector Leap_Tool::GetPos(int i) {
  return tools.at(i)->GetPos();
}

mb::AxisAlignedBoundingBox Leap_Tool::GetBoundingBox(int i) {
  return mb::AxisAlignedBoundingBox(tools.at(i)->GetPos(),10.0f);
}

mb::AxisAlignedBoundingBox Leap_Tool::GetInteractionBox() {
  return mb::AxisAlignedBoundingBox(tools.at(0)->GetPos(),15.0f);
}