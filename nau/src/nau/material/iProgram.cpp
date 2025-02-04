#include "nau/material/iProgram.h"

#include "nau/render/iAPISupport.h"
#ifdef NAU_OPENGL
#include "nau/render/opengl/glProgram.h"
#endif

using namespace nau::render;

//#if NAU_OPENGL_VERSION >= 430
std::vector<std::string> IProgram::ShaderNames = 
	{"Vertex", "Geometry", "Tess Control", "Tess Eval", "Fragment", "Compute", "Task", "Mesh"};
//#elif NAU_OPENGL_VERSION >= 400
//std::string IProgram::ShaderNames[IProgram::SHADER_COUNT] = 
//	{"Vertex", "Geometry", "Tess Control", "Tess Eval", "Fragment"};
//#elif NAU_OPENGL_VERSION >= 320
//std::string IProgram::ShaderNames[IProgram::SHADER_COUNT] = 
//	{"Vertex", "Geometry", "Fragment"};
//#else
//std::string IProgram::ShaderNames[IProgram::SHADER_COUNT] = 
//	{"Vertex", "Fragment"};
//#endif


std::vector<std::string> &
IProgram::GetShaderNames() {
	return ShaderNames;
}


using namespace nau::render;

IProgram*
IProgram::create (void) 
{
#ifdef NAU_OPENGL
	return new GLProgram;
#elif NAU_DIRECTX
	return new DXProgram;
#endif
}


bool 
IProgram::isShaderSupported(IProgram::ShaderType type) {
		
	IAPISupport *sup = IAPISupport::GetInstance();

	if ((type == GEOMETRY_SHADER && !sup->apiSupport(IAPISupport::APIFeatureSupport::GEOMETRY_SHADER)) ||
		((type == TESS_CONTROL_SHADER || type == TESS_EVALUATION_SHADER) && !sup->apiSupport(IAPISupport::APIFeatureSupport::TESSELATION_SHADERS)) ||
		(type == MESH_SHADER && !sup->apiSupport(IAPISupport::APIFeatureSupport::MESH_SHADER)) ||
		(type == TASK_SHADER && !sup->apiSupport(IAPISupport::APIFeatureSupport::TASK_SHADER)))
		return false;
	else
		return true;
}


bool
IProgram::hasTessellationShader() {

	return m_HasTessShader;
}