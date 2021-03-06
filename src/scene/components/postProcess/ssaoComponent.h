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


#ifndef SSAO_COMPONENT_H
#define SSAO_COMPONENT_H

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _RENDER_CAMERA_H
#include "rendering/renderCamera.h"
#endif

namespace Scene
{
   // SSAO: Screen Space Ambient Occlusion

   class SSAOComponent : public Rendering::RenderPostProcess
   {
      private:
         typedef Rendering::RenderPostProcess Parent;

      protected:
         F32 mBias;
         F32 mIntensity;
         F32 mScale;
         F32 mRadius;

         Graphics::ViewTableEntry* mAccumulateView;
         Graphics::ViewTableEntry* mBlurXView;
         Graphics::ViewTableEntry* mBlurYView;
         Graphics::ViewTableEntry* mApplyView;

         bgfx::UniformHandle mSSAOParamsUniform;

         Graphics::Shader* mAccumulateShader;
         Graphics::Shader* mBlurXShader;
         Graphics::Shader* mBlurYShader;
         Graphics::Shader* mApplyShader;

         bgfx::FrameBufferHandle mOcclusionBuffer;
         bgfx::FrameBufferHandle mOcclusionBlurBuffer;

         void initBuffers();
         void destroyBuffers();

      public:
         SSAOComponent();
         ~SSAOComponent();

         virtual void onAddToCamera();
         virtual void onRemoveFromCamera();
         virtual void process();
         virtual void resize();
   };
}

#endif