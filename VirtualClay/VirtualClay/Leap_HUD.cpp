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
			mb::Kernel()->Redraw();
	}
}


Leap_HUD::Leap_HUD():
  menuChoice(this,"menuOptions")
{
    menuChoice.AddItem("Middle_L");
    menuChoice.AddItem("Up_L");
    menuChoice.AddItem("Right_L");
    menuChoice.AddItem("Down_L");
    menuChoice.AddItem("Left_L");    
    menuChoice.AddItem("Middle_R");
    menuChoice.AddItem("Up_R");
    menuChoice.AddItem("Right_R");
    menuChoice.AddItem("Down_R");
    menuChoice.AddItem("Left_R");
    menuChoice.SetValue(0);
    cX = 0;
    cY = 0;
    QString loc = QString("C:/Users/Pedz/Documents/VirtualClay/VirtualClay/VirtualClay/VirtualClay/Resources/");
	  menuMiddle_L = mb::CreateInstance<mb::Texture>();
    menuMiddle_L->CreateFromFile(RESOURCESDIR + QString("menuMiddle_L.png"));
	  menuMiddle_L->SetLocation( mb::TexturePool::locationGPU );
	  menuUp_L = mb::CreateInstance<mb::Texture>();
    menuUp_L->CreateFromFile(RESOURCESDIR + QString("menuUp_L.png"));
	  menuUp_L->SetLocation( mb::TexturePool::locationGPU );
	  menuLeft_L = mb::CreateInstance<mb::Texture>();
    menuLeft_L->CreateFromFile(RESOURCESDIR + QString("menuLeft_L.png"));
	  menuLeft_L->SetLocation( mb::TexturePool::locationGPU );
	  menuDown_L = mb::CreateInstance<mb::Texture>();
    menuDown_L->CreateFromFile(RESOURCESDIR + QString("menuDown_L.png"));
	  menuDown_L->SetLocation( mb::TexturePool::locationGPU );
    menuRight_L = mb::CreateInstance<mb::Texture>();
    menuRight_L->CreateFromFile(RESOURCESDIR + QString("menuRight_L.png"));
	  menuRight_L->SetLocation( mb::TexturePool::locationGPU );
    menuMiddle_R = mb::CreateInstance<mb::Texture>();
    menuMiddle_R->CreateFromFile(RESOURCESDIR + QString("menuMiddle_R.png"));
	  menuMiddle_R->SetLocation( mb::TexturePool::locationGPU );
	  menuUp_R = mb::CreateInstance<mb::Texture>();
    menuUp_R->CreateFromFile(RESOURCESDIR + QString("menuUp_R.png"));
	  menuUp_R->SetLocation( mb::TexturePool::locationGPU );
	  menuLeft_R = mb::CreateInstance<mb::Texture>();
    menuLeft_R->CreateFromFile(RESOURCESDIR + QString("menuLeft_R.png"));
	  menuLeft_R->SetLocation( mb::TexturePool::locationGPU );
	  menuDown_R = mb::CreateInstance<mb::Texture>();
    menuDown_R->CreateFromFile(RESOURCESDIR + QString("menuDown_R.png"));
	  menuDown_R->SetLocation( mb::TexturePool::locationGPU );
    menuRight_R = mb::CreateInstance<mb::Texture>();
    menuRight_R->CreateFromFile(RESOURCESDIR + QString("menuRight_R.png"));
	  menuRight_R->SetLocation( mb::TexturePool::locationGPU );
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
    currTex = menuMiddle_L;
    break;
  case 1:
    currTex = menuUp_L;
    break;
  case 2:
    currTex = menuRight_L;
    break;
  case 3:
    currTex = menuDown_L;
    break;
  case 4:
    currTex = menuLeft_L;
    break;
  case 5:
    currTex = menuMiddle_R;
    break;
  case 6:
    currTex = menuUp_R;
    break;
  case 7:
    currTex = menuRight_R;
    break;
  case 8:
    currTex = menuDown_R;
    break;
  case 9:
    currTex = menuLeft_R;
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
    if( !menuMiddle_L ) {
   //   QString loc = QString("C:/Users/Pedz/Documents/VirtualClay/VirtualClay/VirtualClay/VirtualClay/Resources/");
			//menuMiddle_L = mb::CreateInstance<mb::Texture>();
   //   menuMiddle_L->CreateFromFile(RESOURCESDIR + QString("menuMiddle_L.png"));
	  //  menuMiddle_L->SetLocation( mb::TexturePool::locationGPU );
	  //  menuUp_L = mb::CreateInstance<mb::Texture>();
   //   menuUp_L->CreateFromFile(RESOURCESDIR + QString("menuUp_L.png"));
	  //  menuUp_L->SetLocation( mb::TexturePool::locationGPU );
	  //  menuLeft_L = mb::CreateInstance<mb::Texture>();
   //   menuLeft_L->CreateFromFile(RESOURCESDIR + QString("menuLeft_L.png"));
	  //  menuLeft_L->SetLocation( mb::TexturePool::locationGPU );
	  //  menuDown_L = mb::CreateInstance<mb::Texture>();
   //   menuDown_L->CreateFromFile(RESOURCESDIR + QString("menuDown_L.png"));
	  //  menuDown_L->SetLocation( mb::TexturePool::locationGPU );
   //   menuRight_L = mb::CreateInstance<mb::Texture>();
   //   menuRight_L->CreateFromFile(RESOURCESDIR + QString("menuRight_L.png"));
	  //  menuRight_L->SetLocation( mb::TexturePool::locationGPU );			
   //   menuMiddle_R = mb::CreateInstance<mb::Texture>();
   //   menuMiddle_R->CreateFromFile(RESOURCESDIR + QString("menuMiddle_R.png"));
	  //  menuMiddle_R->SetLocation( mb::TexturePool::locationGPU );
	  //  menuUp_R = mb::CreateInstance<mb::Texture>();
   //   menuUp_R->CreateFromFile(RESOURCESDIR + QString("menuUp_R.png"));
	  //  menuUp_R->SetLocation( mb::TexturePool::locationGPU );
	  //  menuLeft_R = mb::CreateInstance<mb::Texture>();
   //   menuLeft_R->CreateFromFile(RESOURCESDIR + QString("menuLeft_R.png"));
	  //  menuLeft_R->SetLocation( mb::TexturePool::locationGPU );
	  //  menuDown_R = mb::CreateInstance<mb::Texture>();
   //   menuDown_R->CreateFromFile(RESOURCESDIR + QString("menuDown_R.png"));
	  //  menuDown_R->SetLocation( mb::TexturePool::locationGPU );
   //   menuRight_R = mb::CreateInstance<mb::Texture>();
   //   menuRight_R->CreateFromFile(RESOURCESDIR + QString("menuRight_R.png"));
	  //  menuRight_R->SetLocation( mb::TexturePool::locationGPU );
    }
	}
	else
	{
		/*delete menuMiddle_L;
    delete menuUp_L;
    delete menuLeft_L;
    delete menuDown_L;
    delete menuRight_L;
    menuMiddle_L = 0;
		menuUp_L = 0;
		menuLeft_L = 0;
		menuDown_L = 0;
		menuRight_L = 0;
	  delete menuMiddle_R;
    delete menuUp_R;
    delete menuLeft_R;
    delete menuDown_R;
    delete menuRight_R;
		menuMiddle_R = 0;
		menuUp_R = 0;
		menuLeft_R = 0;
		menuDown_R = 0;
		menuRight_R = 0;*/
	};
};

Leap_HUD::~Leap_HUD(void)
{
  	delete menuMiddle_L;
    delete menuUp_L;
    delete menuLeft_L;
    delete menuDown_L;
    delete menuRight_L;
    menuMiddle_L = 0;
		menuUp_L = 0;
		menuLeft_L = 0;
		menuDown_L = 0;
		menuRight_L = 0;
	  delete menuMiddle_R;
    delete menuUp_R;
    delete menuLeft_R;
    delete menuDown_R;
    delete menuRight_R;
		menuMiddle_R = 0;
		menuUp_R = 0;
		menuLeft_R = 0;
		menuDown_R = 0;
		menuRight_R = 0;
}
