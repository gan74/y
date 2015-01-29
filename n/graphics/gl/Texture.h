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

#ifndef N_GRAPHICS_GL_TEXTURE_H
#define N_GRAPHICS_GL_TEXTURE_H

#include <n/graphics/Image.h>
#include <n/defines.h>
#include "GL.h"
#ifndef N_NO_GL

namespace n {
namespace graphics {
namespace gl {

class Texture
{
	struct Data
	{
		Data() : handle(0) {
		}

		~Data() {
			if(handle) {
				glDeleteTextures(1, &handle);
			}
		}

		GLuint handle;
	};

	public:
		Texture(const Image &i);
		Texture();
		~Texture();

		void bind() const;

		bool operator==(const Texture &t) const;
		bool operator!=(const Texture &t) const;

	private:
		friend class ShaderCombinaison;

		int getHandle() const;

		Image image;
		mutable core::SmartPtr<Data> data;
};

}
}
}

#endif

#endif // N_GRAPHICS_GL_TEXTURE_H