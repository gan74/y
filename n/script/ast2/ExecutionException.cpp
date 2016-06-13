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
#include "ExecutionException.h"

namespace n {
namespace script {
namespace ast {

core::String ExecutionException::locString(const core::String &code) const {
	if(index == uint(-1)) {
		return "";
	}
	uint line = 1;
	for(uint i = 0; i != index && i != code.size(); i++) {
		line += code[i] == '\n';
	}
	core::String lineStr("at line ");
	lineStr << core::String2(line) << ": \"";
	core::String str = lineStr;
	uint lineBeg = index;
	for(; lineBeg != 0 && code[lineBeg - 1] != '\n'; lineBeg--);
	uint end = code.find('\n', lineBeg) - code.begin();
	str += code.subString(lineBeg, end - lineBeg) + "\"\n";
	for(uint i = lineBeg; i != index + lineStr.size(); i++) {
		str += "~";
	}
	str += "^";
	return str;
}

ExecutionException::ExecutionException(const core::String &m, uint ind) : msg(m), index(ind) {
}

const char *ExecutionException::what() const noexcept {
	return msg.data();
}

const char *ExecutionException::what(const core::String &code) const noexcept {
	buffer = msg + ":\n" + locString(code);
	return buffer.data();
}


}
}
}