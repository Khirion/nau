#ifndef PASSLIGHTNING_HPP
#define PASSLIGHTNING_HPP

#include <vector>

#include "iNau.h"
#include "nau/render/pass.h"
#include "nau/scene/scene.h"
#include "nau/geometry/mesh.h"
#include "nau/enums.h"

#include "scol.hpp"

class PassLightning : public Pass
		{
		protected:
			int RESTART;
			scol sCol;
			vector<glm::vec3> waypoints;
			bool m_Inited;
			void prepareGeometry();
			void restartGeometry();
			void loadWaypoints();

		public:
			static std::shared_ptr<Pass> Create(const std::string& name);
			PassLightning(const std::string &name);
			~PassLightning(void);

			virtual void init(void);
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

#endif //PASSLIGHTNING_HPP
