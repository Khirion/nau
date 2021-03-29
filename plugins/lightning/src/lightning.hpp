#ifndef PASSGEOMETRY_HPP
#define PASSGEOMETRY_HPP

#include <vector>

#include "scol.hpp"
#include "iNau.h"
#include "nau/render/pass.h"
#include "nau/scene/scene.h"
#include "nau/geometry/mesh.h"

class PassLightning : public Pass
		{
		protected:

			bool m_Inited;
			scol sCol;
			void prepareGeometry();

		public:

			static Pass *Create(const std::string &passName);
			PassLightning(const std::string &name);
			~PassLightning(void);

			virtual void prepare (void);
			virtual void doPass (void);
			virtual void restore (void);


};

extern "C" {
#ifdef WIN32
	__declspec(dllexport) void *createPass(const char *s);
	__declspec(dllexport) void init(void *inau);
	__declspec(dllexport) char *getClassName();
#else
	void *createPass(const char *s);
	void init(void *inau);
	char *getClassName();
#endif	
}

#endif //PASSGEOMETRY_HPP
