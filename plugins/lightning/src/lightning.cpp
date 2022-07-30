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
	cull = m_IntProps[Attribs.get("BRANCH")->getId()];
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

	if (waypoints.size() % 2){
		return;
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

	if (branchpoints.size() % 2) {
		return;
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
	std::shared_ptr<std::vector<unsigned int>> mindices = std::make_shared<std::vector<unsigned int>>(0);
	std::shared_ptr<std::vector<unsigned int>> bindices = std::make_shared<std::vector<unsigned int>>(0);

	// Main Indices
	std::shared_ptr<MaterialGroup> aMaterialGroup = MaterialGroup::Create(m_Renderable.get(), "__Bolt");
	aMaterialGroup->setIndexList(mindices);
	m_Renderable->addMaterialGroup(aMaterialGroup);

	// Branch Indices
	aMaterialGroup = MaterialGroup::Create(m_Renderable.get(), "__Branch");
	aMaterialGroup->setIndexList(bindices);
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

	float partSize = mBranch.getIndiceSize()/ stepTime;
	float ppart = max(1, (partSize * m_FloatProps[TIME]));
	vector<unsigned int> auxi, auxb = vector<unsigned int>(1, (0, 0, 0));
	std::pair<std::vector<unsigned int>, std::vector<unsigned int>> indPair;
	indPair = mBranch.getIndices(ppart);
	if (!indPair.first.empty())
		auxi = indPair.first;
	if(!indPair.second.empty())
		auxb = indPair.second;

	// Main Branch
	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>(auxi);

	// Main Indices
	std::shared_ptr<MaterialGroup> aMaterialGroup = m_Renderable->getMaterialGroups()[0];
	aMaterialGroup->setIndexList(indices);
	aMaterialGroup->resetCompilationFlag();

	// Branch
	// create indices and fill the array
	std::shared_ptr<std::vector<unsigned int>> bindices = std::make_shared<std::vector<unsigned int>>(auxb);

	// Branch Indices
	aMaterialGroup = m_Renderable->getMaterialGroups()[1];
	aMaterialGroup->setIndexList(bindices);
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
		m_IntProps[CONTROL] = static_cast<int>(m_FloatProps[TIME] * 5);
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
PassLightning::genLightning() {
	pblen = m_FloatProps[Attribs.get("PBLENGTH")->getId()];
	sblen = m_FloatProps[Attribs.get("SBLENGTH")->getId()];
	cull = m_IntProps[Attribs.get("BRANCH")->getId()];
	mBranch = mainBranch(m_FloatProps[Attribs.get("CPLX")->getId()],
						 m_FloatProps[Attribs.get("WIDTH")->getId()],
						 m_FloatProps[Attribs.get("WEIGHT")->getId()],
						 static_cast<float>(m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]));

	mBranch.init(waypoints, m_IntProps[Attribs.get("GEN")->getId()]);

	if (cull > 0)
		genBranches();

	if (branchpoints.size() % 2 == 0 && !branchpoints.empty())
		genBways();

	mBranch.makeIndexes();
}
 

void
PassLightning::genBranches() {
	glm::vec3 end;
	std::pair<int, glm::vec3> startPoint;
	std::vector<glm::vec3> mainTree;
	float maxDist = glm::distance(waypoints.front(), waypoints.back());
	float dist = 0.f;
	float cplx = 0.f;

	std::vector<std::pair<int, glm::vec3>> branchNodes = std::vector<std::pair<int, glm::vec3>>(0);
	std::vector<std::pair<int, glm::vec3>> buffer = mBranch.getBranchNodes();
	std::vector<std::pair<int, glm::vec3>> temp = std::vector<std::pair<int, glm::vec3>>(0);


	for (int i = 0; i < cull; i++) {
		branchNodes.assign(buffer.begin(), buffer.end());
		buffer.clear();

		for (int j = 0; j < branchNodes.size(); j++) {
			mainTree = mBranch.getVertices();

			dist = glm::distance(branchNodes[j].second, waypoints.back());

			end = branchNodes[j].second + (glm::normalize(randVec(glm::normalize(waypoints.back() - branchNodes[j].second), dist / maxDist)) * randDist(dist, i));
			branch b = branch(m_FloatProps[Attribs.get("SPLX")->getId()],
				mBranch.getSize(),
				m_FloatProps[Attribs.get("WEIGHT")->getId()],
				m_FloatProps[Attribs.get("WIDTH")->getId()],
				static_cast<float>(m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]),
				i + 1);

			b.init(branchNodes[j].first, branchNodes[j].second, end, mainTree, m_IntProps[Attribs.get("GEN")->getId()]);
			mBranch.addVector(b.getVector());

			temp = b.getBranchNodes();
			buffer.insert(buffer.end(), temp.begin(), temp.end());
		}
	}
}

void
PassLightning::genBways() {
	glm::vec3 end;
	std::pair<int, glm::vec3> startPoint;
	std::vector<glm::vec3> mainTree;
	float maxDist = glm::distance(waypoints.front(), waypoints.back());
	float dist = 0.f;

	int cull = m_IntProps[Attribs.get("BRANCH")->getId()];
	std::vector<std::pair<int, glm::vec3>> branchNodes = std::vector<std::pair<int, glm::vec3>>(0);
	std::vector<std::pair<int, glm::vec3>> buffer = std::vector<std::pair<int, glm::vec3>>(0);
	std::vector<std::pair<int, glm::vec3>> temp = std::vector<std::pair<int, glm::vec3>>(0);


	for (unsigned int i = 0; i < branchpoints.size(); i += 2) {
		startPoint = mBranch.getClosest(waypoints[0] + (glm::normalize(waypoints.back() - waypoints[0]) * branchpoints[i].x));
		mainTree = mBranch.getVertices();

		end = startPoint.second + (glm::normalize(branchpoints[i + 1]) * branchpoints[i].y);

		branch b = branch(branchpoints[i].z,
			mBranch.getSize(),
			m_FloatProps[Attribs.get("WEIGHT")->getId()],
			m_FloatProps[Attribs.get("WIDTH")->getId()],
			static_cast<float>(m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]),
			1);

		b.init(startPoint.first, startPoint.second, end, mainTree, m_IntProps[Attribs.get("GEN")->getId()]);
		mBranch.addVector(b.getVector());

		if (cull > 1) {
			temp = b.getBranchNodes();
			buffer.insert(buffer.end(), temp.begin(), temp.end());
		}
	}

	for (int i = 1; i < cull; i++) {
		branchNodes.assign(buffer.begin(), buffer.end());
		buffer.clear();

		for (int j = 0; j < branchNodes.size(); j++) {
			mainTree = mBranch.getVertices();

			dist = glm::distance(branchNodes[j].second, waypoints.back());

			end = branchNodes[j].second + (glm::normalize(randVec(glm::normalize(waypoints.back() - branchNodes[j].second), dist/maxDist)) * randDist(dist, i));
			branch b = branch(m_FloatProps[Attribs.get("SPLX")->getId()],
				mBranch.getSize(),
				m_FloatProps[Attribs.get("WEIGHT")->getId()],
				m_FloatProps[Attribs.get("WIDTH")->getId()],
				static_cast<float>(m_IntProps[Attribs.get("GROWTH_LENGTH")->getId()]),
				i + 1);

			b.init(branchNodes[j].first, branchNodes[j].second, end, mainTree, m_IntProps[Attribs.get("GEN")->getId()]);
			mBranch.addVector(b.getVector());

			temp = b.getBranchNodes();
			buffer.insert(buffer.end(), temp.begin(), temp.end());
		}
	}
}

glm::vec3 PassLightning::randVec(glm::vec3 vec, float d) {
	// Introduce jaggedness
	static std::default_random_engine gen;
	std::uniform_real_distribution<float> randZ(pi/6, (3*pi)/4);
	std::uniform_real_distribution<float> randY(0, 2*pi);

	

	return glm::rotateY(glm::rotateZ(vec, randZ(gen) * d), randY(gen));
}

float PassLightning::randDist(float dist, int i) {
	static std::default_random_engine generator;
	std::uniform_real_distribution<float> randG(0.f, 1.f);
	auto genR = bind(randG, generator);

	return (i == 0 ? dist * genR() * pblen : (dist * (genR() * 0.25f) * sblen));
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
