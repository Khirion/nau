#ifndef OCTREEBYMATSCENE_H
#define OCTREEBYMATSCENE_H

#include <nau/scene/iscenepartitioned.h>
#include <nau/scene/octreeByMat.h>
#include <nau/geometry/frustum.h>
#include <nau/geometry/boundingbox.h>


namespace nau {
	namespace loader {
		class CBOLoader;
	};
};

namespace nau {

	namespace scene {

		class OctreeByMatScene : public IScenePartitioned
		{
		public:
			friend class nau::loader::CBOLoader;
		private:
			std::vector<SceneObject*> m_vReturnVector;
			std::vector<SceneObject*> m_SceneObjects;

			nau::geometry::BoundingBox m_BoundingBox;

		protected:
			void updateSceneObjectTransforms();
			OctreeByMat *m_pGeometry;

		public:
			OctreeByMatScene (void);
			~OctreeByMatScene (void);

			void clear();

			virtual void eventReceived(const std::string &sender, const std::string &eventType, nau::event_::IEventData *evt);

			virtual void build (void);
			virtual void compile (void);

			virtual nau::geometry::IBoundingVolume& getBoundingVolume (void);

			virtual void unitize();

			virtual void add (nau::scene::SceneObject *aSceneObject);			
			virtual std::vector <SceneObject*>& findVisibleSceneObjects 
																(nau::geometry::Frustum &aFrustum, 
																Camera &aCamera,
																bool conservative = false);
			virtual std::vector<SceneObject*>& getAllObjects ();
			virtual nau::scene::SceneObject* getSceneObject (std::string name);
			virtual nau::scene::SceneObject* getSceneObject (int index);

			virtual void getMaterialNames(std::set<std::string> *nameList);

			virtual nau::math::ITransform *getTransform();
			virtual void setTransform(nau::math::ITransform *t);
			virtual void transform(nau::math::ITransform *t);



			virtual std::string getType (void);

			/*
			 * Statistical information
			 */

			int getNumTriangles() { return 0; };
			int getNumVertices() { return 0; };
		
			// By material
			int getNumTrianglesMat(int i) { return 0; };
			int getNumVerticesMat(int i) { return 0; ;}

			// By Object
			int getNumTrianglesObject(int i) { return 0; };
			int getNumVerticesObject(int i) { return 0; };

			//virtual void scale(float factor) = 0;
			//virtual void translate(float x, float y, float z);
			//virtual void rotate(float ang, float ax, float ay, float az) = 0;
			//void show (void);			
			//void hide (void);
			//bool isVisible (void);
		};
	};
};

#endif
