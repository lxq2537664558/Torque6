//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _GAMEOBJECT_H_
#include "game/gameObject.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _OBJECT_TEMPLATE_H_
#include "scene/objectTemplate.h"
#endif

#ifndef _OBJECT_TEMPLATE_ASSET_H_
#include "scene/objectTemplateAsset.h"
#endif

#ifndef _MTRANSFORM_H_
#include "math/mTransform.h"
#endif

#ifndef _MPLANESET_H_
#include "math/mPlaneSet.h"
#endif

namespace Scene 
{
   class BaseComponent;

   class DLL_PUBLIC SceneObject : public GameObject
   {
      private:
         typedef SimObject Parent;
         bool mAddedToScene;

      public:
         SceneObject();
         ~SceneObject();

         // For now, these are public. 
         ObjectTemplate*         mTemplate;
         StringTableEntry        mTemplateAssetID;
         Vector<BaseComponent*>  mComponents;

         Box3F       mBoundingBox;
         Transform   mTransform;
         bool        mStatic;

         // GameObject
         virtual void processMove( const Move *move );
         virtual void interpolateMove( F32 delta );
         virtual void advanceMove( F32 dt );

         bool isStatic() { return mStatic; }
         bool raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint);
         bool boxSearch(const PlaneSetF& planes);
         virtual void refresh();

         static void initPersistFields();

         static bool setPositionFn(void* obj, const char* data);
         static const char* getPositionFn(void* obj, const char* data);
         static bool setRotationFn(void* obj, const char* data);
         static const char* getRotationFn(void* obj, const char* data);
         static bool setScaleFn(void* obj, const char* data);
         static const char* getScaleFn(void* obj, const char* data);

         virtual void onAddToScene();
         virtual void onRemoveFromScene();
         virtual void onSceneStart();
         virtual void onScenePlay();
         virtual void onScenePause();
         virtual void onSceneStop();

         DECLARE_CONOBJECT(SceneObject);

         template <typename T>
         T* findComponentByType()
         {
            for (S32 n = 0; n < mComponents.size(); ++n)
            {
               T* result = dynamic_cast<T*>(mComponents[n]);
               if (result)
                  return result;
            }

            return NULL;
         }

         SimObject* findComponentByType(const char* pType);
         Vector<SimObject*> findComponentsByType(const char* pType);
         SimObject* findComponent(StringTableEntry internalName);

      protected:
         virtual void onTamlCustomWrite(TamlCustomNodes& customNodes);
         virtual void onTamlCustomRead(const TamlCustomNodes& customNodes);
         static bool setTemplateAsset( void* obj, const char* data ) { static_cast<SceneObject*>(obj)->setTemplateAsset(data); return false; }

      // Networking
      public:
         bool mGhosted;

         enum SceneObjectMasks
         {
            GhostedMask       = BIT( 0 ),
            TemplateMask      = BIT( 1 ),
            TransformMask     = BIT( 2 ),
            ComponentMask     = BIT( 3 ),
            NextFreeMask      = BIT( 4 )
         };

         void writePacketData(GameConnection *conn, BitStream *stream);
         void readPacketData (GameConnection *conn, BitStream *stream);
         U32  packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
         void unpackUpdate(NetConnection* conn, BitStream* stream);
         void setGhosted(bool _value);
         void setTemplateAsset(StringTableEntry assetID);

         void addComponent(BaseComponent* component);
         void removeComponent(BaseComponent* component);
         void clearComponents();

      protected:
         static bool setGhosted(void* obj, const char* data) 
         { 
            bool value;
            Con::setData(TypeBool, &value, 0, 1, &data);
            static_cast<SceneObject*>(obj)->setGhosted(value);
            return false; 
         }
   };
}

#endif