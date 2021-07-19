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

static char className[] = "lightningPI";

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

	PASSFACTORY->registerClass("lightning", Create);
	registerAndInitArrays(Attribs);

	loadWaypoints();

	m_Inited = false;
	m_BoolProps[RESTART] = false;
}

void
PassLightning::loadWaypoints() {
	IBuffer* aBuffer = RESOURCEMANAGER->getBuffer("Waypoints::waypoints");
	unsigned int bsize = aBuffer->getPropui(IBuffer::SIZE);
	float* data = (float*)malloc(bsize);
	aBuffer->getData(0, bsize, data);

	waypoints = vector<glm::vec3>();

	for (int i = 0; i < bsize / 4; i += 3) {
		waypoints.push_back(glm::vec3(data[i], data[i + 1], data[i + 2]));
	}
}

std::shared_ptr<Pass>
PassLightning::Create(const std::string& passName) {

	return dynamic_pointer_cast<Pass>(std::shared_ptr<Pass>(new PassLightning(passName)));
}


PassLightning::PassLightning(const std::string& passName) :
	Pass(passName) {

	m_ClassName = "lightningPI";
	init();
}


PassLightning::~PassLightning(void) {

}

void
PassLightning::prepareGeometry() {
	std::shared_ptr<IScene> m_Scene = RENDERMANAGER->createScene("Lightning", "Scene");

	// create a renderable
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->createRenderable("Mesh", "Lightning");
	m_Renderable->setDrawingPrimitive(nau::render::IRenderable::LINES);

	// fill in vertex array
	vector<glm::vec3> vaux = sCol.getVertices();
	vector<unsigned int> iaux = sCol.getIndices();
	size_t vertexCount = vaux.size();
	
	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(vertexCount));

	for(int i = 0; i < vertexCount; i++){
		vertices->at(i).set(vaux[i].x, vaux[i].y, vaux[i].z);
	}

	std::shared_ptr<VertexData>& vertexData = m_Renderable->getVertexData();

	vertexData->setDataFor(VertexData::GetAttribIndex(std::string("position")), vertices);

	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(iaux);

	// create the material group
	std::shared_ptr<MaterialGroup> aMaterialGroup = MaterialGroup::Create(m_Renderable.get(), "__Emission Blue");
	aMaterialGroup->setIndexList(indices);
	m_Renderable->addMaterialGroup(aMaterialGroup);

	std::shared_ptr<SceneObject>& m_SceneObject = nau::scene::SceneObjectFactory::Create("SimpleObject");

	m_SceneObject->setRenderable(m_Renderable);

	m_Scene->add(m_SceneObject);

	addScene("Lightning");

	m_Inited = true;
}

void
PassLightning::restartGeometry() {
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->getRenderable("Lightning");

	// fill in vertex array
	vector<glm::vec3> vaux = sCol.getVertices();
	vector<unsigned int> iaux = sCol.getIndices();
	size_t vertexCount = vaux.size();

	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(vertexCount));

	for (int i = 0; i < vertexCount; i++) {
		vertices->at(i).set(vaux[i].x, vaux[i].y, vaux[i].z);
	}

	std::shared_ptr<VertexData>& vertexData = m_Renderable->getVertexData();

	vertexData->setDataFor(VertexData::GetAttribIndex(std::string("position")), vertices);
	vertexData->resetCompilationFlag();

	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(iaux);

	// create the material group
	std::shared_ptr<MaterialGroup> aMaterialGroup = m_Renderable->getMaterialGroups()[0];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();

	RENDERMANAGER->getScene("Lightning")->recompile();

	m_BoolProps[RESTART] = false;
}

void
PassLightning::prepare(void) {
	if (!m_Inited) {
		sCol = scol();
        sCol.init(m_FloatProps[Attribs.get("KILL_DST")->getId()], m_FloatProps[Attribs.get("ATT_DST")->getId()],
			m_IntProps[Attribs.get("CHARGES")->getId()], m_IntProps[Attribs.get("SPHERES")->getId()],
			m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()], waypoints);
		prepareGeometry();
	}
	
	if (m_BoolProps[RESTART]) {
		sCol = scol();
		sCol.init(m_FloatProps[Attribs.get("KILL_DST")->getId()], m_FloatProps[Attribs.get("ATT_DST")->getId()],
			m_IntProps[Attribs.get("CHARGES")->getId()], m_IntProps[Attribs.get("SPHERES")->getId()],
			m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()], waypoints);
		restartGeometry();
	}

	prepareBuffers();
	setupCamera();
}


void
PassLightning::restore(void) {

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
