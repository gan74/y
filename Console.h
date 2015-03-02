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

#ifndef CONSOLE
#define CONSOLE

#include <n/concurent/Thread.h>
#include <n/core/Map.h>
#include <n/core/Functor.h>
#include <n/core/String.h>

#include <iostream>


using namespace n;
using namespace n::graphics;
using namespace n::math;
using namespace n::core;

class Console : public n::concurent::Thread
{
	public:
		Console() {
			funcs["print"] = [](String s) -> String {
				std::cout<<s<<std::endl;
				return "";
			};

			funcs["exit"] = [&](String s) -> String {
				one(s);
				save();
				exit(0);
				return "";
			};

			funcs["load"] = [&](String s) -> String {
				load(s);
				return "";
			};

			funcs["save"] = [&](String s) -> String {
				save(s);
				return "";
			};

			load();
		}

		~Console() {
			save();
		}


		virtual void run() override {
			while(true) {
				std::cout<<std::endl<<">>>";
				std::string line;
				std::getline(std::cin, line);
				String cmd = String(line);
				while(!cmd.isEmpty()) {
					cmd = one(cmd);
					if(isNum(cmd)) {
						std::cout<<cmd<<std::endl;
						break;
					}
				}
			}
		}

		String operator()(const String &i) const {
			auto it = vars.find(i);
			if(it == vars.end()) {
				return "";
			}
			return (*it)._2;
		}

	private:
		String one(const String &line) {
			core::Array<String> x = line.split(" ").mapped([](const String &s) { return s.split("="); });
			if(x.isEmpty()) {
				return "";
			}
			String cmd = x.first();
			String rest = removeSpaces(line.subString(cmd.size()));
			auto it = funcs.find(cmd);
			if(it == funcs.end()) {
				if(rest.isEmpty()) {
					return vars[cmd];
				}
				if(rest[0] == '=') {
					rest = removeSpaces(rest.subString(1));
					if(isNum(rest)) {
						return vars[cmd] = rest;
					} else {
						return vars[cmd] = one(rest);
					}
				} else {
					std::cerr<<"Unknown command \""<<rest<<"\""<<std::endl;
					return "";
				}
			} else {
				return (*it)._2(rest);
			}
		}

		static bool isNum(const String &s) {
			bool num = true;
			s.to<float>([&] { num = false; });
			return num;
		}

		static String removeSpaces(String s) {
			while(!s.isEmpty() && isspace(s[0])) {
				s = s.subString(1);
			}
			return s;
		}

		static constexpr auto cfg = "vars.cfg";


		bool load(const String &f = "") {
			io::File file(f.isEmpty() ? cfg : f);
			if(file.open(io::IODevice::Read)) {
				char *data = new char[file.size() + 1];
				file.readBytes(data);
				data[file.size()] = 0;
				file.close();
				for(const String &line : String(data).split("\n")) {
					one(line);
				}
				delete[] data;
				return true;
			}
			std::cerr<<"Unable to load config from \""<<(f.isEmpty() ? cfg : f)<<"\""<<std::endl;
			return false;
		}

		void save(const String &f = "") {
			io::File file(f.isEmpty() ? cfg : f);
			if(file.open(io::IODevice::Write)) {
				for(const Pair<const String, String> &p : vars) {
					file.write(p._1 + "=" + p._2 + "\n");
				}
				file.close();
			}
		}


		Map<String, Functor<String(String)>> funcs;
		Map<String, String> vars;
};

#endif // CONSOLE
