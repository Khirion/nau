#include "lightning.hpp"

#include "iNau.h"
#include "nau.h"
#include "nau/geometry/vertexData.h"
#include "nau/material/iTexture.h"
#include "nau/material/materialGroup.h"
#include "nau/math/vec3.h"
#include "nau/render/passFactory.h"
#include "nau/scene/sceneFactory.h"



#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

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


using namespace nau::geometry;
using namespace nau::math;
using namespace nau::render;
using namespace nau::scene;

Pass*
PassLightning::Create(const std::string& passName) {
	
	return new PassLightning(passName);
}


PassLightning::PassLightning(const std::string& passName) :
	Pass(passName) {

	sCol.init();
	m_ClassName = "lightningPI";
	m_Inited = false;
}


PassLightning::~PassLightning(void) {

}


void
PassLightning::prepareGeometry() {
	std::shared_ptr<IScene> m_Scene = RENDERMANAGER->createScene("my_scene", "Scene");

	// create a renderable
	std::shared_ptr<nau::render::IRenderable>& m_Renderable = RESOURCEMANAGER->createRenderable("Mesh", "lightning");
	m_Renderable->setDrawingPrimitive(nau::render::IRenderable::LINES);

	// fill in vertex array
	vector<glm::vec3> vaux = sCol.getVertices();
	vector<unsigned int> iaux = sCol.getIndices();
	glm::vec3 aux;
	int vertexCount = vaux.size();

	std::shared_ptr<std::vector<VertexData::Attr>> vertices =
		std::shared_ptr<std::vector<VertexData::Attr>>(new std::vector<VertexData::Attr>(vertexCount));

	for(int i = 0; i < vertexCount; i++){
		aux = vaux[i];
		vertices->at(i).set(aux.x, aux.y, aux.z);
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

	addScene("my_scene");

	m_Inited = true;


}


void
PassLightning::prepare(void) {

	if (!m_Inited) {

		prepareGeometry();
	}
	prepareBuffers();
	setupCamera();

}


void
PassLightning::restore(void) {

}


void
PassLightning::doPass(void) {

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

