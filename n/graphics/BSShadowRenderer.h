/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/

#ifndef N_GRAPHICS_BSSHADOWRENDERER
#define N_GRAPHICS_BSSHADOWRENDERER

#include "BufferedRenderer.h"
#include "ShadowRenderer.h"
#include "BlurBufferRenderer.h"

namespace n {
namespace graphics {

class BSShadowRenderer : public ShadowRenderer
{
	public:
		BSShadowRenderer(ShadowRenderer *c, uint fHStep) : ShadowRenderer(c->getFrameBuffer().getSize().x()), child(c), temp(buffer.getSize()), blurs{BlurBufferRenderer::createBlurShader(false, fHStep), BlurBufferRenderer::createBlurShader(true, fHStep)} {
			mapIndex = 0;
			buffer.setDepthEnabled(false);
			buffer.setAttachmentEnabled(0, true);
			buffer.setAttachmentFormat(0, ImageFormat::R32F);
			temp.setDepthEnabled(false);
			temp.setAttachmentEnabled(0, true);
			temp.setAttachmentFormat(0, ImageFormat::R32F);
			blurs[0]->setValue("n_0", child->getShadowMap());
			blurs[1]->setValue("n_0", temp.getAttachement(0));

			shaderCode = "vec3 proj = projectShadow(pos);"
						 "float d = texture(n_LightShadow, proj.xy).x;"
						 "float diff = proj.z - d;"
						 "return pow(1.0 - clamp(diff, 0.0, 1.0), 30.0);";
		}

		~BSShadowRenderer() {
			delete blurs[0];
			delete blurs[1];
		}

		virtual void *prepare() override {
			void *ptr = child->prepare();
			proj = child->getProjectionMatrix();
			view = child->getViewMatrix();
			return ptr;
		}

		virtual void render(void *ptr) override {
			child->render(ptr);

			temp.bind();
			blurs[0]->bind();
			GLContext::getContext()->getScreen().draw(Material(), VertexAttribs(), RenderFlag::NoShader);

			buffer.bind();
			blurs[1]->bind();
			GLContext::getContext()->getScreen().draw(Material(), VertexAttribs(), RenderFlag::NoShader);

			blurs[1]->unbind();
		}

	private:
		ShadowRenderer *child;
		FrameBuffer temp;
		ShaderCombinaison *blurs[2];
};


}
}
#endif // BSSHADOWRENDERER
