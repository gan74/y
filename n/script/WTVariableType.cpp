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
#include "WTVariableType.h"

namespace n {
namespace script {

WTVariableType::WTVariableType(const core::String &typeName, bool obj) : name(typeName), object(obj) {
}

const core::String &WTVariableType::getName() const {
	return name;
}

bool WTVariableType::isObject() const {
	return object;
}

}
}