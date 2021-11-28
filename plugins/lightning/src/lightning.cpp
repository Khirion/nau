#include "nau.h"
#include "nau/geometry/vertexData.h"
#include "nau/material/iTexture.h"
#include "nau/material/materialGroup.h"
#include "nau/math/vec3.h"
#include "nau/render/passFactory.h"
#include "nau/scene/sceneFactory.h"

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "lightning.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

static char className[] = "lightning";

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#elif __linux__
#define EXPORT extern "C"
#endif

EXPORT
void*
createPass(const char* s) {

	std::shared_ptr<PassLightning>* p = new std::shared_ptr<PassLightning>(new PassLightning(s));
	return p;
}


EXPORT
void
init(void* nauInst) {

	INau::SetInterface((nau::INau*)nauInst);
	nau::Nau::SetInstance((nau::Nau*)nauInst);
	glbinding::Binding::initialize(false);
}


EXPORT
char*
getClassName() {

	return className;
}

void
PassLightning::init() {
	RESTART = Attribs.get("RESTART")->getId();
	CONTROL = Attribs.get("CONTROL")->getId();
	TIMECOEF = Attribs.get("TIMECOEF")->getId();
	TIME = Attribs.get("TIME")->getId();
	startTime = 0;
	stepTime = 0.2f;
	m_FloatProps[TIME] = 0.f;

	PASSFACTORY->registerClass("lightning", Create);
	registerAndInitArrays(Attribs);

	loadWaypoints();
	loadBranchpoints();

	m_Inited = false;
	m_BoolProps[RESTART] = false;
	m_IntProps[CONTROL] = 0;
	m_FloatProps[TIMECOEF] = 1.f;
	m_FloatProps[TIME] = 0.f;
}

void
PassLightning::loadWaypoints() {
	IBuffer* aBuffer = RESOURCEMANAGER->getBuffer("Waypoints::waypoints");

	if (aBuffer == NULL) {
		return;
	}

	unsigned int bsize = aBuffer->getPropui(IBuffer::SIZE);
	float* data = (float*)malloc(bsize);
	aBuffer->getData(0, bsize, data);

	waypoints = vector<glm::vec3>();

	for (unsigned int i = 0; i < bsize / 4; i += 3) {
		waypoints.push_back(glm::vec3(data[i], data[i + 1], data[i + 2]));
	}
}

void
PassLightning::loadBranchpoints() {
	IBuffer* aBuffer = RESOURCEMANAGER->getBuffer("Branchpoints::branchpoints");

	if (aBuffer == NULL) {
		return;
	}

	unsigned int bsize = aBuffer->getPropui(IBuffer::SIZE);
	float* data = (float*)malloc(bsize);
	aBuffer->getData(0, bsize, data);

	branchpoints = vector<glm::vec3>();

	for (unsigned int i = 0; i < bsize / 4; i += 3) {
		branchpoints.push_back(glm::vec3(data[i], data[i + 1], data[i + 2]));
	}
}

std::shared_ptr<Pass>
PassLightning::Create(const std::string& passName) {

	return dynamic_pointer_cast<Pass>(std::shared_ptr<Pass>(new PassLightning(passName)));
}


PassLightning::PassLightning(const std::string& passName) :
	Pass(passName) {

	m_ClassName = "lightning";
	init();
}


PassLightning::~PassLightning(void) {

}

void
PassLightning::prepareGeometry() {
	std::shared_ptr<IScene> m_Scene = RENDERMANAGER->createScene("lightning", "Scene");

	// create a renderable
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->createRenderable("Mesh", "lightning");
	m_Renderable->setDrawingPrimitive(nau::render::IRenderable::LINES);

	// fill in vertex array
	vector<glm::vec3> vaux = mBranch.getVertices();
	size_t vertexCount = vaux.size();
	
	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(vertexCount));

	for(int i = 0; i < vertexCount; i++){
		vertices->at(i).set(vaux[i].x, vaux[i].y, vaux[i].z);
	}

	std::shared_ptr<VertexData>& vertexData = m_Renderable->getVertexData();

	vertexData->setDataFor(VertexData::GetAttribIndex(std::string("position")), vertices);

	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(0);

	// Main Indices
	std::shared_ptr<MaterialGroup> aMaterialGroup = MaterialGroup::Create(m_Renderable.get(), "__Bolt");
	aMaterialGroup->setIndexList(indices);
	m_Renderable->addMaterialGroup(aMaterialGroup);

	// Branch Indices
	aMaterialGroup = MaterialGroup::Create(m_Renderable.get(), "__Branch");
	aMaterialGroup->setIndexList(indices);
	m_Renderable->addMaterialGroup(aMaterialGroup);

	std::shared_ptr<SceneObject>& m_SceneObject = nau::scene::SceneObjectFactory::Create("SimpleObject");

	m_SceneObject->setRenderable(m_Renderable);

	m_Scene->add(m_SceneObject);

	addScene("lightning");

	m_IntProps[CONTROL] = 0;
	timeCoef = m_FloatProps[TIMECOEF] * 0.1f;
	m_Inited = true;
}

void
PassLightning::restartGeometry() {
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->getRenderable("lightning");

	// fill in vertex array
	vector<glm::vec3> vaux = mBranch.getVertices();
	size_t vertexCount = vaux.size();

	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(vertexCount));

	for (int i = 0; i < vertexCount; i++) {
		vertices->at(i).set(vaux[i].x, vaux[i].y, vaux[i].z);
	}

	std::shared_ptr<VertexData>& vertexData = m_Renderable->getVertexData();

	vertexData->setDataFor(VertexData::GetAttribIndex(std::string("position")), vertices);
	vertexData->resetCompilationFlag();

	m_IntProps[CONTROL] = 0;
	timeCoef = m_FloatProps[TIMECOEF] * 0.1f;
	m_BoolProps[RESTART] = false;
}

void
PassLightning::iterateGeometry() {
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->getRenderable("lightning");

	vector<unsigned int> iaux = mBranch.getMIndices();
	float partSize = iaux.size() / stepTime;
	unsigned int p = max(1, static_cast<unsigned int>(partSize * m_FloatProps[TIME]));
	vector<unsigned int> subi = vector<unsigned int>(iaux.begin(), iaux.begin() + p);

	// Main Branch
	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(subi);

	// Main Indices
	std::shared_ptr<MaterialGroup> aMaterialGroup = m_Renderable->getMaterialGroups()[0];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();

	iaux = mBranch.getBIndices();
	if (iaux.empty())
		subi = vector<unsigned int>(1, (0,0,0));
	else {
		partSize = iaux.size() / stepTime;
		p = max(0, static_cast<unsigned int>(partSize * m_FloatProps[TIME]));
		subi = vector<unsigned int>(iaux.begin(), iaux.begin() + p);
	}

	// Branch
	// create indices and fill the array
	indices = std::make_shared<std::vector<unsigned int>>(subi);

	// Branch Indices
	aMaterialGroup = m_Renderable->getMaterialGroups()[1];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();

	RENDERMANAGER->getScene("lightning")->recompile();
}

void
PassLightning::cleanGeometry() {
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->getRenderable("lightning");

	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(0));

	std::shared_ptr<VertexData>& vertexData = m_Renderable->getVertexData();

	vertexData->setDataFor(VertexData::GetAttribIndex(std::string("position")), vertices);
	vertexData->resetCompilationFlag();

	vector<unsigned int> subi = vector<unsigned int>(0);

	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(subi);

	// create the material group
	std::shared_ptr<MaterialGroup> aMaterialGroup = m_Renderable->getMaterialGroups()[0];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();

	// create the material group
	aMaterialGroup = m_Renderable->getMaterialGroups()[1];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();


	RENDERMANAGER->getScene("lightning")->recompile();
}

void
PassLightning::prepare(void) {
	if (waypoints.empty())
		return;

	if (!m_Inited) {
		genLightning();
		prepareGeometry();
		startTime = RENDERER->getPropf(RENDERER->TIMER) / CLOCKS_PER_SEC;
	}
	
	if (m_BoolProps[RESTART]) {
		genLightning();
		restartGeometry();
		startTime = RENDERER->getPropf(RENDERER->TIMER) / CLOCKS_PER_SEC;
	}

	m_FloatProps[TIME] = ((RENDERER->getPropf(RENDERER->TIMER) / CLOCKS_PER_SEC) - startTime) * timeCoef;

	if (m_FloatProps[TIME] <= stepTime)
		iterateGeometry();

	if (m_IntProps[CONTROL] <= 5)
		m_IntProps[CONTROL] = static_cast<int>(m_FloatProps[TIME] * 5) - 1;
	else
		cleanGeometry();

	if (0 != m_RenderTarget && true == m_UseRT) {

		if (m_ExplicitViewport) {
			vec2 f2 = m_Viewport[0]->getPropf2(Viewport::ABSOLUTE_SIZE);
			m_RTSizeWidth = (int)f2.x;
			m_RTSizeHeight = (int)f2.y;
			uivec2 uiv2((unsigned int)m_RTSizeWidth, (unsigned int)m_RTSizeHeight);
			m_RenderTarget->setPropui2(IRenderTarget::SIZE, uiv2);
		}
		m_RenderTarget->bind();
	}

	prepareBuffers();
	setupCamera();
}

void
PassLightning::genLightning(void) {
	mBranch = mainBranch(m_FloatProps[Attribs.get("CPLX")->getId()],
						 m_FloatProps[Attribs.get("WIDTH")->getId()],
						 m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]);

	mBranch.init(waypoints);

	if (branchpoints.size() % 2 || branchpoints.empty()) {
		mBranch.makeIndexes();
		return;
	}

	branch b;
	std::pair<glm::vec3, glm::vec3> bway;
	std::pair<int, glm::vec3> startPoint;
	std::vector<glm::vec3> mainTree;

	for (unsigned int i = 0; i < branchpoints.size(); i += 2) {
		startPoint = mBranch.getClosest(waypoints[0] + (glm::normalize(waypoints.back() - waypoints[0]) * branchpoints[i].x));
		mainTree = mBranch.getVertices();

		bway = std::pair<glm::vec3, glm::vec3>(startPoint.second, startPoint.second + (glm::normalize(branchpoints[i + 1]) * branchpoints[i].y));

		b = branch(mBranch.getSize(), 
				   m_FloatProps[Attribs.get("CPLX")->getId()],
				   m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]);

		b.init(startPoint.first, bway, mainTree);
		mBranch.addVector(b.getVector());
	}

	mBranch.makeIndexes();
}


void
PassLightning::restore(void) {
	if (0 != m_RenderTarget && true == m_UseRT) {
		m_RenderTarget->unbind();
	}
}


void
PassLightning::doPass(void) {
	RENDERMANAGER->clearQueue();
	std::vector<std::string>::iterator scenesIter;
	scenesIter = m_SceneVector.begin();

	for (; scenesIter != m_SceneVector.end(); ++scenesIter) {
		std::shared_ptr<IScene>& aScene = RENDERMANAGER->getScene(*scenesIter);
		std::vector<std::shared_ptr<SceneObject>> sceneObjects;
		aScene->getAllObjects(&sceneObjects);
		std::vector<SceneObject*>::iterator objIter;

		for (auto& so : sceneObjects) {

			RENDERMANAGER->addToQueue(so, m_MaterialMap);
		}
	}

	RENDERER->setDepthClamping(true);

	RENDERMANAGER->processQueue();
}

// float t = RENDERER->getPropf(RENDERER->TIMER);
// unsigned int f = RENDERER->getPropui(RENDERER->FRAME_COUNT);
