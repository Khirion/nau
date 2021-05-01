#ifndef PASSLIGHTNING_HPP
#define PASSLIGHTNING_HPP

#include <vector>

#include "iNau.h"
#include "nau/render/pass.h"
#include "nau/scene/scene.h"
#include "nau/geometry/mesh.h"

#include "scol.hpp"

class PassLightning : public Pass
		{
		protected:
			scol sCol;
			void prepareGeometry();

		public:
			FLOAT_PROP(DIST_Y_AVG, 14);
			FLOAT_PROP(DIST_Y_DEV, 15);
			FLOAT_PROP(DIST_XZ_AVG, 16);
			FLOAT_PROP(DIST_XZ_DEV, 17);
			FLOAT_PROP(KILL_DST, 18);

			INT_PROP(CHARGES, 19);
			INT_PROP(GROWTH_LENGTH, 20);

			INTARRAY_PROP(WAYPOINTS, 21);

			BOOL_PROP(RESTART, 22);

			static std::shared_ptr<Pass> Create(const std::string& name);
			PassLightning(const std::string &name);
			~PassLightning(void);

			nau_API void setDistYAvg(float num) { m_FloatProps[DIST_Y_AVG] = num; }
			nau_API void setDistYDev(float num) { m_FloatProps[DIST_Y_DEV] = num; }
			nau_API void setDistXZAvg(float num) { m_FloatProps[DIST_XZ_AVG] = num; }
			nau_API void setDistXZDev(float num) { m_FloatProps[DIST_XZ_DEV] = num; }
			nau_API void setKD(float num) { m_FloatProps[KILL_DST] = num; }
			nau_API void setChargeNum(int num) { m_IntProps[CHARGES] = num; }
			nau_API void setGrowthLength(int num) { m_IntProps[GROWTH_LENGTH] = num; }
			nau_API void setRestart(bool state) { m_BoolProps[RESTART] = state; }
			nau_API void setWaypoints(vector<int> waypoints) { m_IntArrayProps[WAYPOINTS] = waypoints; }

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
