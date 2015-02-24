#include "stdafx.h"
#include "Leap_HUD.h"


IMPLEMENT_VCLASS( Leap_HUD, ViewPortFilter, "LeapHUD", 1 );

static void drawFullScreenQuad(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();     
  glColor3f(1.0f,1.0f,1.0f);
  glBindTexture(GL_TEXTURE_2D,overlay->OpenGLName());

	glBegin( GL_QUADS );
  
	glTexCoord2f( 0, 0 );
	glVertex2f( cX-0.2f, (cY-0.2f)*ratio );
  
	glTexCoord2f( 1, 0 );
	glVertex2f( cX+0.2f, (cY-0.2f)*ratio );
  
	glTexCoord2f( 1, 1 );
	glVertex2f( cX+0.2f, (cY+0.2f)*ratio );
  
	glTexCoord2f( 0, 1 );
	glVertex2f( cX-0.2f, (cY+0.2f)*ratio );

	glEnd();
}



// Renders full screens quad to texture specified
static void gpuTransform(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	outputTexture->SetAsRenderTarget();
  drawFullScreenQuad(outputTexture,overlay,ratio,cX,cY);
	outputTexture->RestoreRenderTarget();
}

void Leap_HUD::OnNodeEvent( const mb::Attribute &cAttribute, mb::NodeEventType eType ) {
	if(eType == mb::etValueChanged) {
		{
			mb::Kernel()->Redraw();
		}
	}
}


Leap_HUD::Leap_HUD():
  menuChoice(this,"menuOptions")
{
    menuChoice.AddItem("Middle");
    menuChoice.AddItem("Up");
    menuChoice.AddItem("Right");
    menuChoice.AddItem("Down");
    menuChoice.AddItem("Left");
    menuChoice.SetValue(0);
    cX = 0;
    cY = 0;
    QString loc = QString("C:/Users/Pedz/Documents/VirtualClay/VirtualClay/VirtualClay/VirtualClay/Resources/");
	  menuMiddle = mb::CreateInstance<mb::Texture>();
    menuMiddle->CreateFromFile(loc + QString("menuMiddle.png"));
	  menuMiddle->SetLocation( mb::TexturePool::locationGPU );
	  menuUp = mb::CreateInstance<mb::Texture>();
    menuUp->CreateFromFile(loc + QString("menuUp.png"));
	  menuUp->SetLocation( mb::TexturePool::locationGPU );
	  menuLeft = mb::CreateInstance<mb::Texture>();
    menuLeft->CreateFromFile(loc + QString("menuLeft.png"));
	  menuLeft->SetLocation( mb::TexturePool::locationGPU );
	  menuDown = mb::CreateInstance<mb::Texture>();
    menuDown->CreateFromFile(loc + QString("menuDown.png"));
	  menuDown->SetLocation( mb::TexturePool::locationGPU );
    menuRight = mb::CreateInstance<mb::Texture>();
    menuRight->CreateFromFile(loc + QString("menuRight.png"));
	  menuRight->SetLocation( mb::TexturePool::locationGPU );
}

void Leap_HUD::SetCentre(mb::Vector &c) {
  cX = c.x;
  cY = c.y;
}

void Leap_HUD::Process(mb::ViewPortState &s) {
  enum mb::Image::Format eFormat = s.m_bHDRNeeded ? mb::Image::e16float : mb::Image::e8integer;

  mb::Texture *currTex = NULL;

  switch(menuChoice) {
  case 0:
    currTex = menuMiddle;
    break;
  case 1:
    currTex = menuUp;
    break;
  case 2:
    currTex = menuRight;
    break;
  case 3:
    currTex = menuDown;
    break;
  case 4:
    currTex = menuLeft;
    break;
  }
  if(currTex == NULL) {
    mblog("NULL TEXTURE!");
  }
  const float cameraAspectRatio = mb::Kernel()->Scene()->ActiveCamera()->AspectRatio();

  // Set up some gl states
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

  gpuTransform(s.m_pColor,currTex,cameraAspectRatio,cX,cY);
  //s.m_pColor = s.m_pColor;
  s.m_pColor = s.m_pColor;
}

void Leap_HUD::SetVisible( bool bVisible )
{
	ViewPortFilter::SetVisible( bVisible );

	if( bVisible )
	{
    if( !menuMiddle ) {
      QString loc = QString("C:/Users/Pedz/Documents/VirtualClay/VirtualClay/VirtualClay/VirtualClay/Resources/");
			menuMiddle = mb::CreateInstance<mb::Texture>();
      menuMiddle->CreateFromFile(loc + QString("menuMiddle.png"));
	    menuMiddle->SetLocation( mb::TexturePool::locationGPU );
	    menuUp = mb::CreateInstance<mb::Texture>();
      menuUp->CreateFromFile(loc + QString("menuUp.png"));
	    menuUp->SetLocation( mb::TexturePool::locationGPU );
	    menuLeft = mb::CreateInstance<mb::Texture>();
      menuLeft->CreateFromFile(loc + QString("menuLeft.png"));
	    menuLeft->SetLocation( mb::TexturePool::locationGPU );
	    menuDown = mb::CreateInstance<mb::Texture>();
      menuDown->CreateFromFile(loc + QString("menuDown.png"));
	    menuDown->SetLocation( mb::TexturePool::locationGPU );
      menuRight = mb::CreateInstance<mb::Texture>();
      menuRight->CreateFromFile(loc + QString("menuRight.png"));
	    menuRight->SetLocation( mb::TexturePool::locationGPU );
    }
	}
	else
	{
		delete menuMiddle;
    delete menuUp;
    delete menuLeft;
    delete menuDown;
    delete menuRight;
		menuMiddle = 0;
		menuUp = 0;
		menuLeft = 0;
		menuDown = 0;
		menuRight = 0;
	};
};

Leap_HUD::~Leap_HUD(void)
{
}
