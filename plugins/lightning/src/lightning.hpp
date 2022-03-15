#ifndef PASSLIGHTNING_HPP
#define PASSLIGHTNING_HPP

#define GLM_FORCE_RADIANS

#include <vector>

#include "iNau.h"
#include "nau/render/pass.h"
#include "nau/scene/scene.h"
#include "nau/geometry/mesh.h"
#include "nau/enums.h"

#include "mainbranch.hpp"
#include "branches.hpp"
#include <../glm/gtx/rotate_vector.hpp>

class PassLightning : public Pass
		{
		protected:
			float pi = (float)3.14159;
			unsigned int RESTART;
			unsigned int TIMECOEF;
			unsigned int CONTROL;
			unsigned int TIME;
			int cull;
			float pblen;
			float sblen;
			float timeCoef;
			float timer;
			float startTime;
			float stepTime;
			mainBranch mBranch;
			vector<glm::vec3> waypoints;
			vector<glm::vec3> branchpoints;
			bool m_Inited;
			void prepareGeometry();
			void iterateGeometry();
			void restartGeometry();
			void cleanGeometry();
			void loadWaypoints();
			void loadBranchpoints();
			void genLightning();
			void genBranches();
			void genBways();
			glm::vec3 randVec(glm::vec3 vec, float d);
			float randDist(float dist, int i);

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
