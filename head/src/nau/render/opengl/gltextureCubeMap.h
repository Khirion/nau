#ifndef GLTEXTURE_CUBEMAP_H
#define GLTEXTURE_CUBEMAP_H

#include "nau/render/textureCubeMap.h"
#include "nau/scene/camera.h"

#include "nau.h"

#include <GL/glew.h>

using namespace nau::render;

namespace nau
{
	namespace render
	{
		class GLTextureCubeMap : public TextureCubeMap
		{
		friend class TextureCubeMap;

		public:

			~GLTextureCubeMap(void);

			virtual void prepare(unsigned int unit, nau::material::TextureSampler *ts);
			virtual void restore(unsigned int unit);

			virtual void build(int immutable = 0);

			virtual void clear();
			virtual void clearLevel(int l);

			virtual void generateMipmaps();


		protected:
			GLTextureCubeMap (std::string label, std::vector<std::string> files, 
				std::string internalFormat,
				std::string aFormat, std::string aType, int width, unsigned char** data, bool mipmap = true );
			virtual int getNumberOfComponents(void);
			virtual int getElementSize(){return 0;};

			static bool InitGL();
			static bool Inited;

			virtual void resize(unsigned int x, unsigned int y, unsigned int z) {};


		private:

			int getIndex(std::string StringArray[], int IntArray[], std::string aString);

			static int faces[6];

			//GLenum translateCoord (TextureCoord aCoord);
		};
	};
};
#endif
