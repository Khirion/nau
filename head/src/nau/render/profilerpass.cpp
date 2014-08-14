#include <nau/render/profilerpass.h>
#include <nau/resource/fontmanager.h>
#include <nau/scene/sceneobject.h>
#include <nau/debug/profile.h>
#include <nau/geometry/mesh.h>




#include <nau.h>



#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


using namespace nau::render;


ProfilerPass::ProfilerPass (const std::string &name) :
	Pass (name), m_pCam(0)
{
	m_ClassName = "profiler";

	m_CameraName = "__ProfilerCamera";
	m_pCam = RENDERMANAGER->getCamera("__ProfilerCamera");
	m_pSO = SceneObjectFactory::create("SimpleObject");
	m_pSO->setRenderable(RESOURCEMANAGER->createRenderable("Mesh", "__ProfilerResult", "Profiler"));

	m_pViewport = NAU->createViewport("__Profiler", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pViewport = new nau::render::Viewport();
	m_pCam->setViewport(m_pViewport);
	m_pViewport->setProp(Viewport::SIZE, vec2(NAU->getWindowWidth(),NAU->getWindowWidth()));
	m_pViewport->setProp(Viewport::ORIGIN, vec2(0,0));
	m_pCam->setProp(Camera::PROJECTION_TYPE, Camera::ORTHO);
	m_pCam->setOrtho(0.0f ,NAU->getWindowWidth() , NAU->getWindowWidth(), 0.0f, -1.0f, 1.0f);

	m_pFont = FontManager::getFont("CourierNew10");

	m_MaterialMap[m_pFont.getMaterialName()] = MaterialID(DEFAULTMATERIALLIBNAME,m_pFont.getMaterialName());

	m_BoolProp[IRenderer::COLOR_CLEAR] = false;
}


ProfilerPass::~ProfilerPass(void)
{
}


void
ProfilerPass::setCamera (const std::string &cameraName) {

}


void
ProfilerPass::prepare (void)
{
	if (0 != m_RenderTarget && true == m_UseRT) {
		m_RenderTarget->bind();
	}

	RENDERER->pushMatrix(IRenderer::PROJECTION_MATRIX);
	RENDERER->loadIdentity(IRenderer::PROJECTION_MATRIX);

	RENDERER->pushMatrix(IRenderer::VIEW_MATRIX);
	RENDERER->loadIdentity(IRenderer::VIEW_MATRIX);



//	vec3 v = m_pViewport->getSize();

	prepareBuffers();

	m_pViewport->setProp(Viewport::SIZE, vec2(NAU->getWindowWidth(),NAU->getWindowHeight()));
	m_pCam->setOrtho(0.0f ,NAU->getWindowWidth() , NAU->getWindowHeight(), 0.0f, -1.0f, 1.0f);

	if (m_pViewport != NULL) {
		RENDERER->setViewport(m_pViewport);
	}
	RENDERER->setCamera(m_pCam);
	
	RENDERER->loadIdentity(IRenderer::MODEL_MATRIX);
	RENDERER->pushMatrix(IRenderer::MODEL_MATRIX);
	RENDERER->translate(IRenderer::MODEL_MATRIX, vec3(15, 15, 0));

}


void
ProfilerPass::restore (void)
{
	if (0 != m_RenderTarget && true == m_UseRT) {
		m_RenderTarget->unbind();
	}

	RENDERER->popMatrix(IRenderer::PROJECTION_MATRIX);
	RENDERER->popMatrix(IRenderer::VIEW_MATRIX);
	RENDERER->popMatrix(IRenderer::MODEL_MATRIX);
}


void 
ProfilerPass::doPass (void)
{
//	nau::scene::SceneObject *string;

	m_pFont.createSentenceRenderable(m_pSO->getRenderable(), Profile::DumpLevels());
	m_pSO->getRenderable().resetCompilationFlags();
	//m_pSO->setRenderable(r);//Profile::DumpLevels()));
	//string = m_pFont.createSentenceSceneObject(Profile::DumpLevels());
	RENDERMANAGER->clearQueue();
	RENDERMANAGER->addToQueue (m_pSO, m_MaterialMap);
	RENDERMANAGER->processQueue();
}

