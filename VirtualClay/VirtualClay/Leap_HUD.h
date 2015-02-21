#pragma once
#include "stdafx.h"
#include <Cg\cg.h>
#include <cmath>
#include <Cg\cgGL.h>
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
  private:
	//mb::CGcontext m_CGContext;
	mb::Texture* m_pResultTexture;
};

