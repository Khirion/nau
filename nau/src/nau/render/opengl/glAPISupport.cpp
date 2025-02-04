#include "nau/render/opengl/glAPISupport.h"

#include <glbinding/gl/gl.h>
using namespace gl;
//#include <GL/glew.h>

#include <string.h>

void 
GLAPISupport::setAPISupport() {

	GLint version, versionAux;

	glGetIntegerv(GL_MAJOR_VERSION, &version);
	glGetIntegerv(GL_MINOR_VERSION, &versionAux);
	m_Version = version * 100 + versionAux * 10;


	//m_Version = 310;

	for (int i = 0; i < static_cast<int>(APIFeatureSupport::OK); ++i) {
		m_APISupport[(APIFeatureSupport)i] = false;
	}

	m_APISupport[APIFeatureSupport::OK] = true;

// https://github.com/cginternals/glbinding/issues/251
//	const char* sExtensions = (char *)glGetString(GL_EXTENSIONS);
//	if (strstr(sExtensions, "ARB_bindless_texture") != NULL) {
//		m_APISupport[APIFeatureSupport::BINDLESS_TEXTURES] = true;
//	}
	int n;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (int i = 0; i < n; ++i) {

		const char* ext = (char*)glGetStringi(GL_EXTENSIONS, i);
		if (strstr(ext, "ARB_bindless_texture") != NULL) {
			m_APISupport[APIFeatureSupport::BINDLESS_TEXTURES] = true;
		}
		if (strstr(ext, "GL_NV_mesh_shader") != NULL) {
			m_APISupport[APIFeatureSupport::MESH_SHADER] = true;
			m_APISupport[APIFeatureSupport::TASK_SHADER] = true;
		}

	}

	if (m_Version >= 310) {
		m_APISupport[APIFeatureSupport::BUFFER_UNIFORM] = true;
	}
	if (m_Version >= 320) {
		m_APISupport[APIFeatureSupport::GEOMETRY_SHADER] = true;
		m_APISupport[APIFeatureSupport::TEXTURE_SAMPLERS] = true;
	}
	if (m_Version >= 400) {
		m_APISupport[APIFeatureSupport::TESSELATION_SHADERS] = true;
	}
	if (m_Version >= 420) {
		m_APISupport[APIFeatureSupport::BUFFER_ATOMICS] = true;
		m_APISupport[APIFeatureSupport::IMAGE_TEXTURE] = true;
		m_APISupport[APIFeatureSupport::TEX_STORAGE] = true;
	}
	if (m_Version >= 430) {
		m_APISupport[APIFeatureSupport::COMPUTE_SHADER] = true;
		m_APISupport[APIFeatureSupport::BUFFER_SHADER_STORAGE] = true;
		m_APISupport[APIFeatureSupport::OBJECT_LABELS] = true;
		m_APISupport[APIFeatureSupport::CLEAR_BUFFER] = true;
	}
	if (m_Version >= 440) {
		m_APISupport[APIFeatureSupport::CLEAR_TEXTURE] = true;
		m_APISupport[APIFeatureSupport::CLEAR_TEXTURE_LEVEL] = true;
		m_APISupport[APIFeatureSupport::RESET_TEXTURES] = true;
	}
}


