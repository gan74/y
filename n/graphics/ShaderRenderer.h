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

#ifndef N_GRAPHICS_SHADERRENDERER
#define N_GRAPHICS_SHADERRENDERER


#include "BufferedRenderer.h"
#include "ShaderCombinaison.h"
#include "Shader.h"

namespace n {
namespace graphics {

class ScreenQuadRenderer : public Renderer
{
	public:
		ScreenQuadRenderer() : Renderer() {
		}

		~ScreenQuadRenderer() {
		}

		virtual void *prepare() override {
			return 0;
		}

		virtual void render(void *) override {
			GLContext::getContext()->getScreen().draw(VertexAttribs());
		}

	private:

};

class ShaderRenderer : public Renderer
{
	public:
		ShaderRenderer(Renderer *c, ShaderCombinaison *s) : Renderer(), child(c), shader(s) {
		}

		~ShaderRenderer() {
		}

		virtual void *prepare() override {
			return child->prepare();
		}

		virtual void render(void *ptr) override {
			shader->bind();
			child->render(ptr);
			shader->unbind();
		}

	private:
		Renderer *child;
		ShaderCombinaison *shader;

};

}
}

#endif // N_GRAPHICS_SHADERRENDERER
