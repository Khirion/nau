#include "nau/render/passQuad.h"

#include "nau.h"
#include "nau/render/passFactory.h"

using namespace nau::render;
using namespace nau::geometry;


bool PassQuad::Inited = PassQuad::Init();


bool
PassQuad::Init() {

	PASSFACTORY->registerClass("quad", Create);
	return true;
}


PassQuad::PassQuad (const std::string &name) :
	Pass (name),
	m_QuadObject (0)
{
	m_ClassName = "quad";
	m_QuadObject = new Quad;
}


PassQuad::~PassQuad(void) {

	delete m_QuadObject;
}


Pass *
PassQuad::Create(const std::string &passName) {

	return new PassQuad(passName);
}


void
PassQuad::prepare (void) {

	if (0 != m_RenderTarget && true == m_UseRT) {
		if (m_ExplicitViewport) {
			vec2 f2 = m_Viewport->getPropf2(Viewport::ABSOLUT_SIZE);
			m_RTSizeWidth = (int)f2.x;
			m_RTSizeHeight = (int)f2.y;
			m_RenderTarget->setPropui2(IRenderTarget::SIZE, uivec2((unsigned int)m_RTSizeWidth, (unsigned int)m_RTSizeWidth));
		}
		m_RenderTarget->bind();
	}

	RENDERER->pushMatrix(IRenderer::PROJECTION_MATRIX);
	RENDERER->loadIdentity(IRenderer::PROJECTION_MATRIX);

	RENDERER->pushMatrix(IRenderer::VIEW_MATRIX);
	RENDERER->loadIdentity(IRenderer::VIEW_MATRIX);

	RENDERER->pushMatrix(IRenderer::MODEL_MATRIX);
	RENDERER->loadIdentity(IRenderer::MODEL_MATRIX);


	if (m_Viewport != NULL)
		RENDERER->setViewport(m_Viewport);

	prepareBuffers();
}


void
PassQuad::restore(void) {

	if (0 != m_RenderTarget && true == m_UseRT) {
		m_RenderTarget->unbind();
	}
	RENDERER->popMatrix(IRenderer::PROJECTION_MATRIX);
	RENDERER->popMatrix(IRenderer::VIEW_MATRIX);
	RENDERER->popMatrix(IRenderer::MODEL_MATRIX);
}


void 
PassQuad::doPass (void) {

	for (auto pp : m_PreProcessList)
		pp->process();

	RENDERMANAGER->clearQueue();
	RENDERMANAGER->addToQueue (m_QuadObject, m_MaterialMap);
	RENDERMANAGER->processQueue();

	for (auto pp : m_PostProcessList)
		pp->process();

}


void 
PassQuad::setMaterialName(std::string &lib, std::string &mat) {

	m_MaterialMap["__Quad"].setMaterialID (lib, mat);
}