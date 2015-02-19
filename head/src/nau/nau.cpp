#include <nau.h>

#include <nau/config.h>
#include <nau/slogger.h>
#include <nau/debug/profile.h>
#include <nau/debug/state.h>
#include <nau/event/eventFactory.h>
#include <nau/loader/cboloader.h>
#include <nau/loader/objLoader.h>
#include <nau/loader/ogremeshloader.h>
#include <nau/loader/assimploader.h>
#include <nau/loader/patchLoader.h>
#include <nau/loader/projectloader.h>
#ifdef GLINTERCEPTDEBUG
#include <nau/loader/projectloaderdebuglinker.h>
#endif //GLINTERCEPTDEBUG
#include <nau/resource/fontmanager.h>
#include <nau/scene/scenefactory.h>
#include <nau/system/file.h>
#include <nau/world/worldfactory.h>

#include <GL/glew.h>

#ifdef NAU_LUA
extern "C" {
#include<lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}
#endif

#include <ctime>

// added for directory loading
#ifdef NAU_PLATFORM_WIN32
#include <dirent.h>
#else
#include <dirent.h>
#include <sys/types.h>
#endif

using namespace nau;
using namespace nau::system;
using namespace nau::loader;
using namespace nau::scene;
using namespace nau::render;
using namespace nau::resource;
using namespace nau::world;
using namespace nau::material;


static nau::Nau *gInstance = 0;

nau::Nau*
Nau::create (void) {
	if (0 == gInstance) {
		gInstance = new Nau;
	}
	
	return gInstance;
}


nau::Nau*
Nau::getInstance (void) {
	if (0 == gInstance) {
		create();
	}

	return gInstance;
}


Nau::Nau() :
	m_WindowWidth (0.0f), 
	m_WindowHeight (0.0f), 
//	m_vViewports(),
	m_Inited (false),
	m_Physics (false),
	loadedScenes(0),
	m_ActiveCameraName(""),
	m_Name("Nau"),
	m_RenderFlags(COUNT_RENDER_FLAGS),
	m_UseTangents(false),
	m_UseTriangleIDs(false),
	m_CoreProfile(false),
	isFrameBegin(true), 
	m_DummyVector()
{
}


Nau::~Nau() {

	delete MATERIALLIBMANAGER;
	EVENTMANAGER->clear();
	RENDERMANAGER->clear();
	RESOURCEMANAGER->clear();

}


bool 
Nau::init (bool context, std::string aConfigFile) {

	//bool result;
	if (true == context) {

		m_pRenderManager = new RenderManager;
		m_pEventManager = new EventManager;
	}	
	
	m_pResourceManager = new ResourceManager ("."); /***MARK***/ //Get path!!!
	m_pMaterialLibManager = new MaterialLibManager();

	try {
		ProjectLoader::loadMatLib("./nauSystem.mlib");
	}
	catch (std::string s) {
		clear();
		throw(s);
	}

	FontManager::addFont("CourierNew10", "./couriernew10.xml", "__FontCourierNew10");

	m_pWorld = WorldFactory::create ("Bullet");

	CLOCKS_PER_MILISEC = CLOCKS_PER_SEC / 1000.0;
	INV_CLOCKS_PER_MILISEC = 1.0 / CLOCKS_PER_MILISEC;

	m_CurrentTime = clock() * INV_CLOCKS_PER_MILISEC;
	m_LastFrameTime = NO_TIME;

	m_Inited = true;

	EVENTMANAGER->addListener("WINDOW_SIZE_CHANGED",this);

	m_DefaultState = IState::create();
	m_Viewport = m_pRenderManager->createViewport("defaultFixedVP");

	State::init();

	// Init LUA
#ifdef NAU_LUA
	initLua();
#endif


	return true;
}


std::string&
Nau::getName() {

	return(m_Name);
}


// ----------------------------------------------------
//		Lua Stuff
// ----------------------------------------------------

#ifdef NAU_LUA

// Must do something when the parameters are invalid
int
luaGet(lua_State *l) {

	const char *tipo = lua_tostring(l, -5);
	const char *context = lua_tostring(l, -4);
	const char *component = lua_tostring(l, -3);
	int number = lua_tonumber(l, -2);
	void *arr;
	AttribSet *attr = NAU->getAttribs(tipo);
	std::string s = component;
	Enums::DataType dt, bdt;
	int id;
	attr->getPropTypeAndId(s, &dt, &id);
	int card = Enums::getCardinality(dt);
	bdt = Enums::getBasicType(dt);
	arr = NAU->getAttribute(tipo, context, component, number);
	float *arrF;

	switch (bdt) {

	case Enums::FLOAT:
		arrF = (float *)arr;
		for (int i = 0; i < card ; ++i) {
			lua_pushnumber(l, i + 1); // key
			lua_pushnumber(l, arrF[i]); // value
			lua_settable(l, -3); // two pushes 
		}
		break;
	}
	return 0;
}

int 
luaSet(lua_State *l) {

	const char *tipo = lua_tostring(l, -5);
	const char *context = lua_tostring(l, -4);
	const char *component = lua_tostring(l, -3);
	int number = lua_tonumber(l, - 2);
	void *arr;
	AttribSet *attr = NAU->getAttribs(tipo);
	std::string s = component;
	Enums::DataType dt, bdt;
	int id;
	attr->getPropTypeAndId(s, &dt, &id);
	int card = Enums::getCardinality(dt);
	bdt = Enums::getBasicType(dt);
	float *arrF;
	int *arrI; 
	unsigned int *arrUI;

	switch (bdt) {

	case Enums::FLOAT:
		arrF = (float *)malloc(sizeof(float) * card);
		lua_pushnil(l);
		for (int i = 0; i < card && lua_next(l,-2) != 0; ++i) {
			arrF[i] = lua_tonumber(l, -1);
			lua_pop(l, 1);
		}
		arr = arrF;
		break;
	case Enums::INT:
	case Enums::BOOL:
		arrI = (int *)malloc(sizeof(int) * card);
		lua_pushnil(l);
		for (int i = 0; i < card && lua_next(l, -2) != 0; ++i) {
			arrI[i] = lua_tonumber(l, -1);
			lua_pop(l, 1);
		}
		arr = arrI;
		break;
	case Enums::UINT :
		arrUI = (unsigned int *)malloc(sizeof(unsigned int) * card);
		lua_pushnil(l);
		for (int i = 0; i < card && lua_next(l, -2) != 0; ++i) {
			arrUI[i] = lua_tounsigned(l, -1);
			lua_pop(l, 1);
		}
		arr = arrUI;
		break;
	}

	NAU->setAttribute(tipo, context, component, number, arr);
	return 0;
}


void 
Nau::initLua() {

	m_LuaState = luaL_newstate();
	luaL_openlibs(m_LuaState);
	lua_pushcfunction(m_LuaState, luaSet);
	lua_setglobal(m_LuaState, "set");
	lua_pushcfunction(m_LuaState, luaGet);
	lua_setglobal(m_LuaState, "get");
}


void
Nau::initLuaScript(std::string file, std::string name) {

	int res = luaL_dofile(m_LuaState, file.c_str());
	if (res) {
		NAU_THROW("Error loading lua file: %s", lua_tostring(m_LuaState, -1));
	}
}


void
Nau::callLuaScript(std::string file, std::string name) {

	lua_getglobal(m_LuaState, name.c_str());
	lua_pcall(m_LuaState, 0, 0, 0);

	// com envio de parāmetros
	//lua_pushnumber(m_LuaState, 12);
	//lua_pushlightuserdata(m_LuaState, &file);
	//lua_pcall(m_LuaState, 2, 0, 0);

	// receber parāmetros
	//lua_pcall(m_LuaState, 0, 1, 0);
	//int k = lua_tonumber(m_LuaState, -1);

}

#endif


// ----------------------------------------------------------
//		GET AND SET ATTRIBUTES
// ----------------------------------------------------------


AttributeValues *
Nau::getObjectAttributes(std::string type, std::string context, int number) {

	// From Render Manager
	if (type == "CAMERA") {
		if (m_pRenderManager->hasCamera(context))
			return (AttributeValues *)m_pRenderManager->getCamera(context);
	}
	if (type == "LIGHT") {
		if (m_pRenderManager->hasLight(context))
			return (AttributeValues *)m_pRenderManager->getLight(context);
	}
	if (type == "SCENE") {
		if (m_pRenderManager->hasScene(context))
			return (AttributeValues *)m_pRenderManager->getScene(context);
	}
	if (type == "PASS") {
		std::string pipName = m_pRenderManager->getActivePipelineName();
		if (m_pRenderManager->hasPass(pipName, context))
			return (AttributeValues *)m_pRenderManager->getPass(pipName, context);
	}
	if (type == "VIEWPORT") {
		if (m_pRenderManager->hasViewport(context))
			return (AttributeValues *)m_pRenderManager->getViewport(context);
	}

	// From ResourceManager

	if (type == "BUFFER") {
		if (m_pResourceManager->hasBuffer(context))
			return (AttributeValues *)m_pResourceManager->getBuffer(context);
	}
	if (type == "STATE") {
		if (m_pResourceManager->hasState(context))
			return (AttributeValues *)m_pResourceManager->getState(context);
	}

	// From Materials

	std::string lib, mat;
	std::size_t found = context.find("::");
	if (found != std::string::npos && context.size() > found+2) {
		lib = context.substr(0, found);
		mat = context.substr(found + 2);
	}

	if (type == "COLOR_MATERIAL") {
		if (m_pMaterialLibManager->hasMaterial(lib, mat))
			return (AttributeValues *)&(m_pMaterialLibManager->getMaterial(lib, mat)->getColor());
	}
	if (type == "TEXTURE") {
		if (m_pMaterialLibManager->hasMaterial(lib, mat))
			return (AttributeValues *)m_pMaterialLibManager->getMaterial(lib, mat)->getTexture(number);
	}
	if (type == "IMAGE_TEXTURE") {
		if (m_pMaterialLibManager->hasMaterial(lib, mat))
			return (AttributeValues *)m_pMaterialLibManager->getMaterial(lib, mat)->getImageTexture(number);
	}
	if (type == "MATERIAL_BUFFER") {
		if (m_pMaterialLibManager->hasMaterial(lib, mat))
			return (AttributeValues *)m_pMaterialLibManager->getMaterial(lib, mat)->getBuffer(number);
	}
	if (type == "TEXTURE_SAMPLER") {
		if (m_pMaterialLibManager->hasMaterial(lib, mat))
			return (AttributeValues *)m_pMaterialLibManager->getMaterial(lib, mat)->getTextureSampler(number);
	}


	// If we get here then we are trying to fetch something that does not exist
	assert(false && "Getting an invalid attribute - Nau::getObject");
	return NULL;
}


bool
Nau::validateAttribute(std::string type, std::string context, std::string component) {

	//if (type == "CAMERA") {
	//
	//	if (!m_pRenderManager->hasCamera(context))
	//		return false;
	//}
	if (!getObjectAttributes(type, context))
		return false;
	return ProgramValue::Validate(type, context, component);
}


void 
Nau::setAttribute(std::string type, std::string context, std::string component, int number, void *values) {

	int id;
	Enums::DataType dt; 
	AttributeValues *attrVal;

	m_Attributes[type]->getPropTypeAndId(component, &dt, &id);
	attrVal = getObjectAttributes(type, context, number);

	if (attrVal == NULL) {
		assert(false && "Invalid parameters Nau::getAttribute");
	}
	else
		attrVal->setProp(id, dt, values);
}


void *
Nau::getAttribute(std::string type, std::string context, std::string component, int number) {

	int id;
	Enums::DataType dt;
	AttributeValues *attrVal;

	m_Attributes[type]->getPropTypeAndId(component, &dt, &id);
	attrVal = getObjectAttributes(type, context, number);

	if (attrVal == NULL) {
		assert(false && "Invalid parameters Nau::getAttribute");
		return NULL;
	}
	else
		return attrVal->getProp(id, dt);
}


// -----------------------------------------------------------
//		USER ATTRIBUTES
// -----------------------------------------------------------


void 
Nau::registerAttributes(std::string s, AttribSet *attrib) {

	m_Attributes[s] = attrib;
}


bool 
Nau::validateUserAttribContext(std::string context) {

	if (m_Attributes.count(context) != 0)
		return true;
	else
		return false;
}


AttribSet *
Nau::getAttribs(std::string context) {

	if (m_Attributes.count(context) != NULL)
		return m_Attributes[context];
	else
		return NULL;
}


bool 
Nau::validateUserAttribName(std::string context, std::string name) {

	AttribSet *attribs = getAttribs(context);

 // invalid context
	if (attribs == NULL)
		return false;

	Attribute a = attribs->get(name);
	if (a.getName() == "NO_ATTR")
		return true;
	else
		return false;
}


void
Nau::deleteUserAttributes() {

	for (auto attr : m_Attributes) {

		attr.second->deleteUserAttributes();
	}
}


std::vector<std::string> &
Nau::getContextList() {

	m_DummyVector.clear();
	for (auto attr : m_Attributes) {
		m_DummyVector.push_back(attr.first);
	}
	return m_DummyVector;
}


// -----------------------------------------------------------
//		EVENTS
// -----------------------------------------------------------


void
Nau::eventReceived(const std::string &sender, const std::string &eventType, IEventData *evtData) {

	if (eventType == "WINDOW_SIZE_CHANGED") {
	
		vec3 *evVec = (vec3 *)evtData->getData();
		setWindowSize(evVec->x,evVec->y);
	}
}


void
Nau::readProjectFile (std::string file, int *width, int *height) {

	try {
		ProjectLoader::load (file, width, height);
		isFrameBegin = true;
	}
	catch (std::string s) {
		clear();
		throw(s);
	}

	setActiveCameraName(RENDERMANAGER->getDefaultCameraName());
}


void
Nau::readDirectory (std::string dirName) {

	DIR *dir;
	struct dirent *ent;
	bool result = true;
	char fileName [1024];
	char sceneName[256];

	clear();
	sprintf(sceneName,"MainScene"); //,loadedScenes);
	loadedScenes++;
	RENDERMANAGER->createScene (sceneName);
	dir = opendir (dirName.c_str());

	if (0 == dir) {
		NAU_THROW("Can't open dir: %s",dirName);
	}
	while (0 != (ent = readdir (dir))) {

#ifdef NAU_PLATFORM_WIN32
		sprintf (fileName, "%s\\%s", dirName.c_str(), ent->d_name);
#else
		sprintf (fileName, "%s/%s", dirName, ent->d_name);						
#endif
		try {
			NAU->loadAsset (fileName, sceneName);
		}
		catch(std::string &s) {
			closedir(dir);
			throw(s);
		}
	}
	closedir (dir);
	loadFilesAndFoldersAux(sceneName,false);	
}


void
Nau::readModel (std::string fileName) throw (std::string) {

	clear();
	bool result = true;
	
	char sceneName[256];

	sprintf(sceneName,"MainScene"); //,loadedScenes);
	loadedScenes++;

	RENDERMANAGER->createScene (sceneName);

	try {
		NAU->loadAsset (fileName, sceneName);
		loadFilesAndFoldersAux(sceneName, false);
	} 
	catch (std::string &s) {
			throw(s);
	}
}


void
Nau::appendModel(std::string fileName) {
	
	char sceneName[256];

	sprintf(sceneName,"MainScene"); //,loadedScenes);
	loadedScenes++;

	RENDERMANAGER->createScene (sceneName);

	try {
		NAU->loadAsset (fileName, sceneName);
		loadFilesAndFoldersAux(sceneName, false);
	}
	catch( std::string &s){
		throw(s);
	}
}


void Nau::loadFilesAndFoldersAux(char *sceneName, bool unitize) {

	Camera *aNewCam = m_pRenderManager->getCamera ("MainCamera");
	Viewport *v = m_pRenderManager->getViewport("defaultFixedVP");//createViewport ("MainViewport", nau::math::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	aNewCam->setViewport (v);

	setActiveCameraName("MainCamera");

	if (unitize) {
		aNewCam->setPerspective (60.0f, 0.01f, 100.0f);
		aNewCam->setPropf4(Camera::POSITION, 0.0f, 0.0f, 5.0f, 1.0f);
	}
	else
		aNewCam->setPerspective (60.0f, 1.0f, 10000.0f);

	// creates a directional light by default
	Light *l = m_pRenderManager->getLight("MainDirectionalLight");
	l->setPropf4(Light::DIRECTION,1.0f,-1.0f,-1.0f, 0.0f);
	l->setPropf4(Light::COLOR, 0.9f,0.9f,0.9f,1.0f);
	l->setPropf4(Light::AMBIENT,0.5f,0.5f,0.5f,1.0f );

	Pipeline *aPipeline = m_pRenderManager->getPipeline("MainPipeline");
	Pass *aPass = aPipeline->createPass("MainPass");
	aPass->setCamera ("MainCamera");

	aPass->setViewport (v);
	aPass->setPropb(Pass::COLOR_CLEAR, true);
	aPass->setPropb(Pass::DEPTH_CLEAR, true);
	aPass->addLight ("MainDirectionalLight");

	aPass->addScene(sceneName);
	m_pRenderManager->setActivePipeline("MainPipeline");
//	RENDERMANAGER->prepareTriangleIDsAndTangents(true,true);
}


bool
Nau::reload (void) {

	if (false == m_Inited) {
		return false;
	}
	return true;
}


void
Nau::clear() {

	resetFrameCount();
	setActiveCameraName("");
	SceneObject::ResetCounter();
	MATERIALLIBMANAGER->clear();
	EVENTMANAGER->clear();
	EVENTMANAGER->addListener("WINDOW_SIZE_CHANGED", this);
	RENDERMANAGER->clear();
	RESOURCEMANAGER->clear();

	deleteUserAttributes();

	// Need to clear font manager

	//while (!m_vViewports.empty()){

	//	m_vViewports.erase(m_vViewports.begin());
	//}

	m_Viewport = RENDERMANAGER->createViewport("defaultFixedVP");

	ProjectLoader::loadMatLib("./nauSystem.mlib");
}


void Nau::step() {

	IRenderer *renderer = RENDERER;
	m_CurrentTime = clock() * INV_CLOCKS_PER_MILISEC;
	if (NO_TIME == m_LastFrameTime) {
		m_LastFrameTime = m_CurrentTime;
	}
	double deltaT = m_CurrentTime - m_LastFrameTime;
	m_LastFrameTime = m_CurrentTime;

#ifdef GLINTERCEPTDEBUG
	addMessageToGLILog("\n#NAU(FRAME,START)");
#endif //GLINTERCEPTDEBUG

	m_pEventManager->notifyEvent("FRAME_BEGIN", "Nau", "", NULL);


	renderer->resetCounters();
	RESOURCEMANAGER->clearBuffers();

	if (true == m_Physics) {
		m_pWorld->update();
		m_pEventManager->notifyEvent("DYNAMIC_CAMERA", "MainCanvas", "", NULL);
	}

	m_pRenderManager->renderActivePipeline();

	m_pEventManager->notifyEvent("FRAME_END", "Nau", "", NULL);

	if (m_FrameCount == ULONG_MAX)
		// 2 avoid issues with run_once and skip_first
		// and allows a future implementation of odd and even frames for
		// ping-pong rendering
		m_FrameCount = 2;
	else
		++m_FrameCount;

}


void Nau::stepPass() {

	IRenderer *renderer = RENDERER;
	m_CurrentTime = clock() * INV_CLOCKS_PER_MILISEC;
	if (NO_TIME == m_LastFrameTime) {
		m_LastFrameTime = m_CurrentTime;
	}
	double deltaT = m_CurrentTime - m_LastFrameTime;
	m_LastFrameTime = m_CurrentTime;

	Pipeline *p;
	p = RENDERMANAGER->getActivePipeline();
	int lastPass;
	int currentPass;
	lastPass = p->getNumberOfPasses()-1;
	currentPass = p->getPassCounter();

	// if it is the first pass
	if (currentPass == 0) {

		m_pEventManager->notifyEvent("FRAME_BEGIN", "Nau", "", NULL);

#ifdef GLINTERCEPTDEBUG
		addMessageToGLILog("\n#NAU(FRAME,START)");
#endif //GLINTERCEPTDEBUG

		renderer->resetCounters();

		if (true == m_Physics) {
			m_pWorld->update();
			m_pEventManager->notifyEvent("DYNAMIC_CAMERA", "MainCanvas", "", NULL);
		}

	}

#ifdef GLINTERCEPTDEBUG
	std::string s = RENDERMANAGER->getCurrentPass()->getName();
	addMessageToGLILog(("\n#NAU(PASS,START," + s + ")").c_str());
#endif //GLINTERCEPTDEBUG

	p->executeNextPass();

#ifdef GLINTERCEPTDEBUG
	addMessageToGLILog(("\n#NAU(PASS,END," + s + ")").c_str());
#endif //GLINTERCEPTDEBUG

	if (currentPass == lastPass) {

		m_pEventManager->notifyEvent("FRAME_END", "Nau", "", NULL);
//#ifdef GLINTERCEPTDEBUG
//		addMessageToGLILog(("\n#NAU(PASS,END," + s + ")").c_str());
//#endif //GLINTERCEPTDEBUG	
	
	}

	if (m_FrameCount == ULONG_MAX)
		// 2 avoid issues with run_once and skip_first
		// and allows a future implementation of odd and even frames for
		// ping-pong rendering
		m_FrameCount = 2;
	else
		++m_FrameCount;
}


void 
Nau::stepCompleteFrame() {

	Pipeline *p;
	p = RENDERMANAGER->getActivePipeline();
	int totalPasses;
	int currentPass;
	totalPasses = p->getNumberOfPasses();
	currentPass = p->getPassCounter();

	for (int i = currentPass; i < totalPasses; ++i)
		stepPass();
}


void Nau::stepPasses(int n) {

	for (int i = 0; i < n; ++i)
		stepPass();
}


void 
Nau::resetFrameCount() {

	m_FrameCount = 0;
}


unsigned long
Nau::getFrameCount() {

	return m_FrameCount;

}


void 
Nau::setActiveCameraName(const std::string &aCamName) {

	if (m_ActiveCameraName != "") {
		EVENTMANAGER->removeListener("CAMERA_MOTION",RENDERMANAGER->getCamera(m_ActiveCameraName));
		EVENTMANAGER->removeListener("CAMERA_ORIENTATION",RENDERMANAGER->getCamera(m_ActiveCameraName));
	}
	m_ActiveCameraName = aCamName;

	if (m_ActiveCameraName != "") {
		EVENTMANAGER->addListener("CAMERA_MOTION",RENDERMANAGER->getCamera(m_ActiveCameraName));
		EVENTMANAGER->addListener("CAMERA_ORIENTATION",RENDERMANAGER->getCamera(m_ActiveCameraName));
	}
}


nau::scene::Camera *
Nau::getActiveCamera() {

	if (RENDERMANAGER->hasCamera(m_ActiveCameraName)) {
		return RENDERMANAGER->getCamera(m_ActiveCameraName);
	}
	else
		return NULL;
}


float
Nau::getDepthAtCenter() {

	float f;
	vec2 v = m_Viewport->getPropf2(Viewport::ORIGIN);
	v += m_Viewport->getPropf2(Viewport::SIZE);
	Camera *cam = RENDERMANAGER->getCamera(m_ActiveCameraName);
	float a = cam->getPropm4(Camera::PROJECTION_MATRIX).at(2,2);
	float b = cam->getPropm4(Camera::PROJECTION_MATRIX).at(3,2);
	// This must move to glRenderer!!!!
	f = RENDERER->getDepthAtPoint(v.x/2, v.y/2);
	f = (f-0.5) * 2;
	SLOG("Depth %f %f\n",b/(f+a), f); 
	return (0.5 * (-a*f + b) / f + 0.5);
}


void 
Nau::sendKeyToEngine (char keyCode) {

	switch(keyCode) {
	case 'K':
		Profile::Reset();
		break;
	case 'I':
		getDepthAtCenter();
		break;
	}
}


void 
Nau::setClickPosition(int x, int y) {

	m_ClickX = x;
	m_ClickY = y;
}


int
Nau::getClickX() {

	return m_ClickX;
}


int
Nau::getClickY() {

	return m_ClickY;
}


IWorld&
Nau::getWorld (void) {

	return (*m_pWorld);
}


void
Nau::loadAsset (std::string aFilename, std::string sceneName, std::string params) throw (std::string) {

	File file (aFilename);

	try {
		switch (file.getType()) {
			case File::PATCH:
				PatchLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath());
				break;
			case File::COLLADA:
			case File::BLENDER:
			case File::PLY:
			case File::LIGHTWAVE:
			case File::STL:
			case File::TRUESPACE:
				AssimpLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath(),params);
				break;
			case File::NAUBINARYOBJECT:
				CBOLoader::loadScene(RENDERMANAGER->getScene(sceneName), file.getFullPath());
				break;
			case File::THREEDS:
				AssimpLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath(), params);
				//THREEDSLoader::loadScene (RENDERMANAGER->getScene (sceneName), file.getFullPath(),params);				
				break;
			case File::WAVEFRONTOBJ:
				AssimpLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath(),params);
				//OBJLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath());				
				break;
			case File::OGREXMLMESH:
				OgreMeshLoader::loadScene(RENDERMANAGER->getScene (sceneName), file.getFullPath());				
				break;
			default:
			  break;
		}
	}
	catch(std::string &s) {
		throw(s);
	}

	Profile::Reset();
}


void
Nau::writeAssets (std::string fileType, std::string aFilename, std::string sceneName) {

	if (0 == fileType.compare ("CBO")) {
		CBOLoader::writeScene (RENDERMANAGER->getScene (sceneName), aFilename);
	}
}

void
Nau::setWindowSize (float width, float height) {

	m_Viewport->setPropf2(Viewport::SIZE, vec2(width,height));
	m_WindowWidth = width;
	m_WindowHeight = height;
}


float 
Nau::getWindowHeight() {

	return(m_WindowHeight);
}


float 
Nau::getWindowWidth() {

	return(m_WindowWidth);
}


//Viewport*
//Nau::createViewport (const std::string &name, nau::math::vec4 &bgColor) {
//
//	Viewport* v = new Viewport;
//
//	v->setName(name);
//	v->setPropf2 (Viewport::ORIGIN, vec2(0.0f,0.0f));
//	v->setPropf2 (Viewport::SIZE, vec2(m_WindowWidth, m_WindowHeight));
//
//	v->setPropf4(Viewport::CLEAR_COLOR, bgColor);
//	v->setPropb(Viewport::FULL, true);
//
//	m_vViewports[name] = v;
//
//	return v;
//}
//
//
//bool 
//Nau::hasViewport(const std::string &name) {
//
//	return (m_vViewports.count(name) != NULL);
//}
//
//
//Viewport*
//Nau::createViewport (const std::string &name) {
//
//	Viewport* v = new Viewport;
//
//	v->setName(name);
//	v->setPropf2 (Viewport::ORIGIN, vec2(0.0f,0.0f));
////	v->setPropf2 (Viewport::SIZE, vec2(m_WindowWidth, m_WindowHeight));
//	v->setPropb(Viewport::FULL, true);
//
//	m_vViewports[name] = v;
//
//	return v;
//}


//Viewport* 
//Nau::getViewport (const std::string &name) {
//
//	if (m_vViewports.count(name))
//		return m_vViewports[name];
//	else
//		return NULL;
//}


Viewport*
Nau::getDefaultViewport() {
	
	return m_Viewport;
}


//std::vector<std::string> *
//Nau::getViewportNames() {
//
//	std::vector<std::string> *names = new std::vector<std::string>; 
//
//	for( std::map<std::string, nau::render::Viewport*>::iterator iter = m_vViewports.begin(); iter != m_vViewports.end(); ++iter ) {
//      names->push_back(iter->first); 
//    }
//	return names;
//}


void
Nau::enablePhysics(void) {

	m_Physics = true;
}


void
Nau::disablePhysics (void) {

	m_Physics = false;
}


void
Nau::setRenderFlag(RenderFlags aFlag, bool aState) {

	m_RenderFlags[aFlag] = aState;
}


bool
Nau::getRenderFlag(RenderFlags aFlag) {

	return(m_RenderFlags[aFlag]);
}


int 
Nau::picking (int x, int y, std::vector<nau::scene::SceneObject*> &objects, nau::scene::Camera &aCamera) {

	return -1;//	RenderManager->pick (x, y, objects, aCamera);
}

//StateList functions:
void 
Nau::loadStateXMLFile(std::string file){
	State::loadStateXMLFile(file);
}


std::vector<std::string> 
Nau::getStateEnumNames() {
	return State::getStateEnumNames();
}


std::string 
Nau::getState(std::string enumName) {
	return State::getState(enumName);
}




RenderManager* 
Nau::getRenderManager (void) {

	return m_pRenderManager;
}


ResourceManager* 
Nau::getResourceManager (void) {

	return m_pResourceManager;
}


MaterialLibManager*
Nau::getMaterialLibManager (void) {

	return m_pMaterialLibManager;
}


EventManager*
Nau::getEventManager (void) {

	return m_pEventManager;
}


