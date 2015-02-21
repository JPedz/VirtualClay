#include "stdafx.h"
#include "Leap_HUD.h"


IMPLEMENT_VCLASS( Leap_HUD, ViewPortFilter, "HUD Filter", 1 );

static void drawFullScreenQuad() {
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glBegin( GL_QUADS );

	glTexCoord2f( 0, 0 );
	glVertex2f( -1, -1 );

	glTexCoord2f( 1, 0 );
	glVertex2f( 1, -1 );

	glTexCoord2f( 1, 1 );
	glVertex2f( 1, 1 );

	glTexCoord2f( 0, 1 );
	glVertex2f( -1, 1 );

	glEnd();
}

// Renders full screens quad to texture specified
static void gpuTransform(mb::Texture* const outputTexture) {
	outputTexture->SetAsRenderTarget();
	drawFullScreenQuad();
	outputTexture->RestoreRenderTarget();
}

void Leap_HUD::OnNodeEvent( const mb::Attribute &cAttribute, mb::NodeEventType eType ) {
	if(eType == mb::etValueChanged) {
		{
			mb::Kernel()->Redraw();
		}
	}
}


Leap_HUD::Leap_HUD(void)
{

	m_pResultTexture = mb::CreateInstance<mb::Texture>();
}

void Leap_HUD::Process(mb::ViewPortState &s) {
  enum mb::Image::Format eFormat = s.m_bHDRNeeded ? mb::Image::e16float : mb::Image::e8integer;
	// Set of variables not actually used for anything other than to show
	// how to access the values.
	const float cameraNear = mb::Kernel()->Scene()->ActiveCamera()->Near();
	const float cameraFar = mb::Kernel()->Scene()->ActiveCamera()->Far();
	const float cameraFOV = mb::Kernel()->Scene()->ActiveCamera()->FOV();
	const float cameraAspectRatio = mb::Kernel()->Scene()->ActiveCamera()->AspectRatio();
	const float cameraHeight = tanf(cameraFOV / 2.0f) * cameraNear;
	const float cameraWidth = cameraHeight * cameraAspectRatio;
	const float cameraTop = -cameraHeight;
	const float cameraBottom = cameraHeight;
	const float cameraLeft = -cameraWidth;
	const float cameraRight = cameraWidth;
  m_pResultTexture->CreateFromFile(QString("C:/Users/Pedz/Pictures/kitten.png"));
	m_pResultTexture->SetLocation( mb::TexturePool::locationGPU );

  mblog("\nImageWidth = "+QString::number(m_pResultTexture->Width())+"\n");
  // Set up some gl states
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

  gpuTransform(m_pResultTexture);
	s.m_pColor = m_pResultTexture;
}

void Leap_HUD::SetVisible( bool bVisible )
{
	ViewPortFilter::SetVisible( bVisible );

	if( bVisible )
	{
		if( !m_pResultTexture )
			m_pResultTexture = mb::CreateInstance<mb::Texture>();
      m_pResultTexture->CreateFromFile(QString("C:/Users/Pedz/Pictures/kitten.png"));
	    m_pResultTexture->SetLocation( mb::TexturePool::locationGPU );
	}
	else
	{
		delete m_pResultTexture;
		m_pResultTexture = 0;
	};
};

Leap_HUD::~Leap_HUD(void)
{
}
