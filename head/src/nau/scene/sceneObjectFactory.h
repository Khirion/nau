#ifndef SCENEOBJECTFACTORY_H
#define SCENEOBJECTFACTORY_H

//#include "nau/scene/sceneObject.h"

#include <string>


namespace nau
{
	namespace scene
	{
		class SceneObject;

		class SceneObjectFactory
		{
		public:
			static nau::scene::SceneObject* Create (std::string type);
		private:
			SceneObjectFactory(void) {};
			~SceneObjectFactory(void) {};
		};
	};
};
#endif
