#include "stdafx.h"
#include "Leap_HUD.h"


IMPLEMENT_VCLASS( Leap_HUD, ViewPortFilter, "LeapHUD", 1 );

static void drawFullScreenQuad(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();     
  glColor3f(1.0f,1.0f,1.0f);
  if(overlay != NULL)
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

static void drawBrushSize(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float size) {
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();     
  glColor3f(1.0f,1.0f,1.0f);
  if(overlay != NULL)
    glBindTexture(GL_TEXTURE_2D,overlay->OpenGLName());

	glBegin( GL_QUADS );
  
	glTexCoord2f( 0, 0 );
	glVertex2f( 0-size, (0-size)*ratio );
  
	glTexCoord2f( 1, 0 );
	glVertex2f( 0+size, (0-size)*ratio );
  
	glTexCoord2f( 1, 1 );
	glVertex2f( 0+size, (0+size)*ratio );
  
	glTexCoord2f( 0, 1 );
	glVertex2f( 0-size, (0+size)*ratio );

	glEnd();
}



// Renders full screens quad to texture specified
static void gpuTransform(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio, const float cX, const float cY) {
	outputTexture->SetAsRenderTarget();
  drawFullScreenQuad(outputTexture,overlay,ratio,cX,cY);
	outputTexture->RestoreRenderTarget();
}

static void gpuTransform(mb::Texture* const outputTexture,mb::Texture* const overlay, const float ratio,const float size) {
	outputTexture->SetAsRenderTarget();
  drawBrushSize(outputTexture,overlay,ratio,size);
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
    menuChoice.AddItem("BrushSize");
    
    menuChoice.AddItem("Up_L_0");
    menuChoice.AddItem("Up_L_1");
    menuChoice.AddItem("Right_L_0");
    menuChoice.AddItem("Right_L_1");
    menuChoice.AddItem("Up_R_0");
    menuChoice.AddItem("Up_R_1");
    menuChoice.AddItem("Right_R_0");
    menuChoice.AddItem("Right_R_1");
    menuChoice.AddItem("Down_R_0");
    menuChoice.AddItem("Down_R_1");
    menuChoice.AddItem("BrushStrength");
    menuChoice.SetValue(0);
    cX = 0;
    cY = 0;
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
    brushSize = mb::CreateInstance<mb::Texture>();
    brushSize->CreateFromFile(RESOURCESDIR + QString("brushsize.png"));
	  brushSize->SetLocation( mb::TexturePool::locationGPU );


    
	  menuUp_L_0 = mb::CreateInstance<mb::Texture>();
    menuUp_L_0->CreateFromFile(RESOURCESDIR + QString("menuUp_L_0.png"));
	  menuUp_L_0->SetLocation( mb::TexturePool::locationGPU );
	  menuUp_L_1 = mb::CreateInstance<mb::Texture>();
    menuUp_L_1->CreateFromFile(RESOURCESDIR + QString("menuUp_L_1.png"));
	  menuUp_L_1->SetLocation( mb::TexturePool::locationGPU );
    menuRight_L_0 = mb::CreateInstance<mb::Texture>();
    menuRight_L_0->CreateFromFile(RESOURCESDIR + QString("menuRight_L_0.png"));
	  menuRight_L_0->SetLocation( mb::TexturePool::locationGPU );
    menuRight_L_1 = mb::CreateInstance<mb::Texture>();
    menuRight_L_1->CreateFromFile(RESOURCESDIR + QString("menuRight_L_1.png"));
	  menuRight_L_1->SetLocation( mb::TexturePool::locationGPU );
    
	  menuUp_R_0 = mb::CreateInstance<mb::Texture>();
    menuUp_R_0->CreateFromFile(RESOURCESDIR + QString("menuUp_R_0.png"));
	  menuUp_R_0->SetLocation( mb::TexturePool::locationGPU );
	  menuUp_R_1 = mb::CreateInstance<mb::Texture>();
    menuUp_R_1->CreateFromFile(RESOURCESDIR + QString("menuUp_R_1.png"));
	  menuUp_R_1->SetLocation( mb::TexturePool::locationGPU );
	  menuDown_R_0 = mb::CreateInstance<mb::Texture>();
    menuDown_R_0->CreateFromFile(RESOURCESDIR + QString("menuDown_R_0.png"));
	  menuDown_R_0->SetLocation( mb::TexturePool::locationGPU );
	  menuDown_R_1 = mb::CreateInstance<mb::Texture>();
    menuDown_R_1->CreateFromFile(RESOURCESDIR + QString("menuDown_R_1.png"));
	  menuDown_R_1->SetLocation( mb::TexturePool::locationGPU );
    menuRight_R_0 = mb::CreateInstance<mb::Texture>();
    menuRight_R_0->CreateFromFile(RESOURCESDIR + QString("menuRight_R_0.png"));
	  menuRight_R_0->SetLocation( mb::TexturePool::locationGPU );
    menuRight_R_1 = mb::CreateInstance<mb::Texture>();
    menuRight_R_1->CreateFromFile(RESOURCESDIR + QString("menuRight_R_1.png"));
	  menuRight_R_1->SetLocation( mb::TexturePool::locationGPU );
}

void Leap_HUD::SetCentre(mb::Vector &c) {
  mblog("LEAPHUD "+VectorToQStringLine(c));
  cX = MIN(MAX(c.x,-0.5),0.5);
  cY = MIN(MAX(c.y,-0.3),0.3);
  mblog("LEAPHUD "+QString::number(cX)+" "+QString::number(cY)+"\n");
}

void Leap_HUD::SetSize(float s) {
  size = s;
}

void Leap_HUD::Process(mb::ViewPortState &s) {
  //enum mb::Image::Format eFormat = s.m_bHDRNeeded ? mb::Image::e16float : mb::Image::e8integer;

  mb::Texture *currTex = NULL;

  switch(menuChoice) {
  case 0:
    currTex = menuMiddle_L;
    break;
  case 1:
    currTex = menuUp_L;
    break;
    case 2:
      mblog("MenuRight_L\n");
    currTex = menuRight_L;
    break;
  case 3:
    currTex = menuDown_L;
    break;
    case 4:
      mblog("MenuLeft_L\n");
    currTex = menuLeft_L;
    break;
  case 5:
    currTex = menuMiddle_R;
    break;
  case 6:
    currTex = menuUp_R;
    break;
  case 7:
      mblog("MenuRight_R\n");
    currTex = menuRight_R;
    break;
  case 8:
    currTex = menuDown_R;
    break;
  case 9:
    currTex = menuLeft_R;
    break;
  case 10:
    currTex = brushSize;
    break;  
  case 11:
    currTex = menuUp_L_0;
    break;  
  case 12:
    currTex = menuUp_L_1;
    break;  
  case 13:
    currTex = menuRight_L_0;
    break;  
  case 14:
    currTex = menuRight_L_1;
    break;  
  case 15:
    currTex = menuUp_R_0;
    break;  
  case 16:
    currTex = menuUp_R_1;
    break;  
  case 17:
    currTex = menuRight_R_0;
    break;  
  case 18:
    currTex = menuRight_R_1;
    break;
  case 19:
    currTex = menuDown_R_0;
    break;  
  case 20:
    currTex = menuDown_R_1;
    break; 
  case 21:
    currTex = brushSize;
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
  if((menuChoice == 10) || (menuChoice == 21)) {
    gpuTransform(s.m_pColor,currTex,cameraAspectRatio,size);
  } else {
    gpuTransform(s.m_pColor,currTex,cameraAspectRatio,cX,cY);
  }
  s.m_pColor = s.m_pColor;
}

void Leap_HUD::SetVisible( bool bVisible )
{
  if(bVisible != ViewPortFilter::Visible()) {
	  ViewPortFilter::SetVisible( bVisible );
  }

	//if( bVisible )
	//{
 //   if( !menuMiddle_L ) {
 //  //   QString loc = QString("C:/Users/Pedz/Documents/VirtualClay/VirtualClay/VirtualClay/VirtualClay/Resources/");
	//		//menuMiddle_L = mb::CreateInstance<mb::Texture>();
 //  //   menuMiddle_L->CreateFromFile(RESOURCESDIR + QString("menuMiddle_L.png"));
	//  //  menuMiddle_L->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuUp_L = mb::CreateInstance<mb::Texture>();
 //  //   menuUp_L->CreateFromFile(RESOURCESDIR + QString("menuUp_L.png"));
	//  //  menuUp_L->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuLeft_L = mb::CreateInstance<mb::Texture>();
 //  //   menuLeft_L->CreateFromFile(RESOURCESDIR + QString("menuLeft_L.png"));
	//  //  menuLeft_L->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuDown_L = mb::CreateInstance<mb::Texture>();
 //  //   menuDown_L->CreateFromFile(RESOURCESDIR + QString("menuDown_L.png"));
	//  //  menuDown_L->SetLocation( mb::TexturePool::locationGPU );
 //  //   menuRight_L = mb::CreateInstance<mb::Texture>();
 //  //   menuRight_L->CreateFromFile(RESOURCESDIR + QString("menuRight_L.png"));
	//  //  menuRight_L->SetLocation( mb::TexturePool::locationGPU );			
 //  //   menuMiddle_R = mb::CreateInstance<mb::Texture>();
 //  //   menuMiddle_R->CreateFromFile(RESOURCESDIR + QString("menuMiddle_R.png"));
	//  //  menuMiddle_R->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuUp_R = mb::CreateInstance<mb::Texture>();
 //  //   menuUp_R->CreateFromFile(RESOURCESDIR + QString("menuUp_R.png"));
	//  //  menuUp_R->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuLeft_R = mb::CreateInstance<mb::Texture>();
 //  //   menuLeft_R->CreateFromFile(RESOURCESDIR + QString("menuLeft_R.png"));
	//  //  menuLeft_R->SetLocation( mb::TexturePool::locationGPU );
	//  //  menuDown_R = mb::CreateInstance<mb::Texture>();
 //  //   menuDown_R->CreateFromFile(RESOURCESDIR + QString("menuDown_R.png"));
	//  //  menuDown_R->SetLocation( mb::TexturePool::locationGPU );
 //  //   menuRight_R = mb::CreateInstance<mb::Texture>();
 //  //   menuRight_R->CreateFromFile(RESOURCESDIR + QString("menuRight_R.png"));
	//  //  menuRight_R->SetLocation( mb::TexturePool::locationGPU );
 //   }
	//}
	//else
	//{
	//	/*delete menuMiddle_L;
 //   delete menuUp_L;
 //   delete menuLeft_L;
 //   delete menuDown_L;
 //   delete menuRight_L;
 //   menuMiddle_L = 0;
	//	menuUp_L = 0;
	//	menuLeft_L = 0;
	//	menuDown_L = 0;
	//	menuRight_L = 0;
	//  delete menuMiddle_R;
 //   delete menuUp_R;
 //   delete menuLeft_R;
 //   delete menuDown_R;
 //   delete menuRight_R;
	//	menuMiddle_R = 0;
	//	menuUp_R = 0;
	//	menuLeft_R = 0;
	//	menuDown_R = 0;
	//	menuRight_R = 0;*/
	//};
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
