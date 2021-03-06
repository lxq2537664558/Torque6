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


#ifndef _DEFERRED_SHADING_H_
#define _DEFERRED_SHADING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _RENDER_CAMERA_H_
#include "rendering/renderCamera.h"
#endif

namespace Rendering 
{
   class DeferredShading : public RenderPath
   {
      protected:
         // BackBuffer
         bgfx::FrameBufferHandle    mBackBuffer;
         bgfx::TextureHandle        mBackBufferTexture;
         bgfx::TextureHandle        mDepthBufferRead;

         // GBuffer
         bgfx::TextureHandle        mGBufferTextures[4];

         // Decals
         bgfx::FrameBufferHandle    mDecalBuffer;

         // Light Buffer
         bgfx::TextureHandle        mLightBufferTextures[2];
         bgfx::FrameBufferHandle    mDiffuseLightBuffer;
         bgfx::FrameBufferHandle    mSpecularLightBuffer;
         bgfx::FrameBufferHandle    mLightBuffer;

         // Final Stage.
         bgfx::FrameBufferHandle    mFinalBuffer;
         Graphics::Shader*          mCombineShader; 
         
         Graphics::ViewTableEntry*  mBackBufferView;
         Graphics::ViewTableEntry*  mDeferredGeometryView;
         Graphics::ViewTableEntry*  mDeferredDecalView;
         Graphics::ViewTableEntry*  mDeferredDiffuseLightView;
         Graphics::ViewTableEntry*  mDeferredSpecularLightView;
         Graphics::ViewTableEntry*  mDeferredLightView;
         Graphics::ViewTableEntry*  mDeferredFinalView;

         S32                        mDeferredMaterialVariantIndex;

      public:
         bgfx::FrameBufferHandle    mGBuffer;
         Graphics::Shader*          mDefaultShader;

         DeferredShading(RenderCamera* camera);
         ~DeferredShading();

         void init();
         void destroy();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void resize();

         bool                       hasDiffuseLightBuffer()       { return true; }
         Graphics::ViewTableEntry*  getDiffuseLightBufferView()   { return mDeferredDiffuseLightView; }
         bool                       hasSpecularLightBuffer()      { return true; }
         Graphics::ViewTableEntry*  getSpecularLightBufferView()  { return mDeferredSpecularLightView; }
         bool                       hasLightBuffer()              { return true; }
         Graphics::ViewTableEntry*  getLightBufferView()          { return mDeferredLightView; }

         // Render Targets
         bgfx::FrameBufferHandle getBackBuffer();
         bgfx::TextureHandle     getColorTexture();
         bgfx::TextureHandle     getDepthTexture();
         bgfx::TextureHandle     getDepthTextureRead();
         bgfx::TextureHandle     getNormalTexture();
         bgfx::TextureHandle     getMatInfoTexture();

         DECLARE_RENDER_PATH("DeferredShading", DeferredShading);
   };
}

#endif