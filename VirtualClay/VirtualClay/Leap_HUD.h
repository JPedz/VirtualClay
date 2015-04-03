#pragma once
#include "stdafx.h"
#include <Cg/cg.h>
#include <cmath>
#include <Cg/cgGL.h>
#include <QtCore/QDir>
class Leap_HUD : public mb::ViewPortFilter {
public:
  DECLARE_CLASS
  Leap_HUD(void);
  ~Leap_HUD(void);
  void SetVisible(bool vis);
  RequirementValue Requirement( void ) const { return eHDR | ePosition32 | eNormal16; };
	void Process( mb::ViewPortState & );
	virtual QString Name( const mb::ClassDesc * ) const { return "HUD Filter"; };
	void OnNodeEvent(const mb::Attribute &cAttribute, mb::NodeEventType eType);
  mb::aenum menuChoice;
  mb::Vector menuPosition;
  void SetCentre(mb::Vector &c);
  void SetSize(float s);
  private:
  float cX;
  float size;
  float cY;
	//mb::CGcontext m_CGContext;
  mb::Texture* brushSize;
  mb::Texture* brushStrength;
	mb::Texture* menuMiddle_L;
	mb::Texture* menuUp_L;
	mb::Texture* menuRight_L;
	mb::Texture* menuDown_L;
	mb::Texture* menuLeft_L;
	mb::Texture* menuUp_L_0;
	mb::Texture* menuUp_L_1;
	mb::Texture* menuRight_L_0;
	mb::Texture* menuRight_L_1;
	mb::Texture* menuMiddle_R;
	mb::Texture* menuUp_R;
	mb::Texture* menuRight_R;
	mb::Texture* menuDown_R;
	mb::Texture* menuLeft_R;
	mb::Texture* menuUp_R_0;
	mb::Texture* menuUp_R_1;
	mb::Texture* menuRight_R_0;
	mb::Texture* menuRight_R_1;
	mb::Texture* menuDown_R_0;
	mb::Texture* menuDown_R_1;
};

