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

#include "ShaderCombinaison.h"
#include "FrameBuffer.h"

namespace n {
namespace graphics {


ShaderCombinaison::ShaderCombinaison(Shader<FragmentShader> *frag, Shader<VertexShader> *vert, Shader<GeometryShader> *geom) : handle(0), bindings(0) {
	shaders[FragmentShader] = frag;
	shaders[VertexShader] = vert;
	shaders[GeometryShader] = geom;
	compile();
}

ShaderCombinaison::ShaderCombinaison(Shader<FragmentShader> *frag, ShaderProgram::StandardVertexShader vs) : ShaderCombinaison(frag, ShaderProgram::getDefaultVertexShader(vs)) {
}

ShaderCombinaison::~ShaderCombinaison() {
	if(handle) {
		gl::GLuint h = handle;
		GLContext::getContext()->addGLTask([=]() {
			gl::glDeleteProgram(h);
		});
	}
	delete[] bindings;
}

void ShaderCombinaison::bind() {
	if(GLContext::getContext()->shader == this) {
		return;
	}
	gl::glUseProgram(handle);
	for(const core::Pair<const core::String, BlockInfo> &i : blocks) {
		const BlockInfo &infos = i._2;
		if(infos.buffer && infos.addr != gl::GLuint(GL_INVALID_INDEX)) {
			infos.buffer->update();
			gl::glBindBufferBase(GL_UNIFORM_BUFFER, infos.slot, infos.buffer->handle);
		}
	}
	for(uint i = 0; i != samplers.size(); i++) {
		bindings[i].bind(i);
	}

	setValue("n_ModelMatrix", GLContext::getContext()->getModelMatrix());
	setValue("n_ProjectionMatrix", GLContext::getContext()->getProjectionMatrix());
	setValue("n_ViewMatrix", GLContext::getContext()->getViewMatrix());
	setValue("n_ViewProjectionMatrix", GLContext::getContext()->getProjectionMatrix() * GLContext::getContext()->getViewMatrix());

	GLContext::getContext()->shader = this;
	GLContext::getContext()->program = 0;
}

void ShaderCombinaison::compile() {
	handle = gl::glCreateProgram();
	bool val = true;
	for(uint i = 0; i != 3; i++) {
		if(shaders[i]) {
			gl::glAttachShader(handle, shaders[i]->handle);
			val &= shaders[i]->isValid();
			if(!shaders[i]->getLogs().isEmpty()) {
				logs += shaders[i]->getLogs() + "\n";
			}
		}
	}
	gl::glLinkProgram(handle);
	int res = 0;
	gl::glGetProgramiv(handle, GL_LINK_STATUS, &res);
	if(!res || !val) {
		int size = 0;
		gl::glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &size);
		char *msg = new char[size + 1];
		gl::glGetProgramInfoLog(handle, size, &res, msg);
		gl::glDeleteProgram(handle);
		handle = 0;
		msg[size] = '\0';
		logs = msg;
		delete[] msg;
		throw ShaderLinkingException(logs);
	} else {
		getUniforms();
	}
}

void ShaderCombinaison::getUniforms() {
	const uint max = 512;
	char name[max];
	int uniforms = 0;
	gl::glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &uniforms);
	for(uint i = 0; i != (uint)uniforms; i++) {
		gl::GLsizei size = 0;
		gl::GLenum type = GL_NONE;
		gl::glGetActiveUniform(handle, i, max - 1, 0, &size, &type, name);
		core::String uniform = name;
		if(uniform.endsWith("[0]")) {
			uniform = uniform.subString(0, uniform.size() - 3);
		}
		UniformInfo info({gl::glGetUniformLocation(handle, name), (uint)size});
		uniformsInfo[uniform] = info;
		if(type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) {
			uint slot = samplers.size();
			setValue(info.addr, int(slot));
			samplers[info.addr] = slot;
		}
	}
	gl::GLint outs = 0;
	gl::glGetProgramInterfaceiv(handle, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &outs);
	gl::GLenum prop = GL_LOCATION;
	gl::GLsizei len = 0;
	for(gl::GLint i = 0; i != outs; i++) {
		gl::GLint loc = 0;
		gl::glGetProgramResourceiv(handle, GL_PROGRAM_OUTPUT, i, 1, &prop, 1, &len, &loc);
		if(loc != -1) {
			outputs.append(loc);
		}
	}
	bindings = new internal::TextureBinding[samplers.size()];
}



}
}