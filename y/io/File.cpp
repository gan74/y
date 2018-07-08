/*******************************
Copyright (c) 2016-2018 Grégoire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/
#include "File.h"

namespace y {
namespace io {

static void file_not_opened() {
	y_throw("File not opened.");
}

static void check_c_err(int errcode) {
	if(errcode) {
		y_throw("Unknown error.");
	}
}

File::File(std::FILE* f) : _file(f) {
}

File::~File() {
	if(_file) {
		std::fclose(_file);
	}
}

File::File(File&& other) {
	swap(other);
}

File& File::operator=(File&& other) {
	swap(other);
	return *this;
}

void File::swap(File& other) {
	std::swap(_file, other._file);
}

core::Result<File> File::create(std::string_view name) {
	FILE* file = std::fopen(name.data(), "wb+");
	if(file) {
		return core::Ok<File>(file);
	}
	return core::Err();
}

core::Result<File, void> File::open(std::string_view name) {
	std::FILE* file = std::fopen(name.data(), "rb");
	if(file) {
		return core::Ok<File>(file);
	}
	return core::Err();
}

bool File::exists(std::string_view name) {
	std::FILE* file = std::fopen(name.data(), "rb");
	if(file) {
		std::fclose(file);
	}
	return file;
}

bool File::copy(std::string_view src, std::string_view dst) {
	FILE* src_file = std::fopen(src.data(), "rb");
	if(!src_file) {
		return false;
	}
	auto src_close = scope_exit([=] { std::fclose(src_file); });

	FILE* dst_file = std::fopen(dst.data(), "wb+");
	if(!dst_file) {
		return false;
	}
	auto dst_close = scope_exit([=] { std::fclose(dst_file); });

	char buffer[1024];
	usize read_size = 0;
	do {
		read_size = std::fread(buffer, 1, sizeof(buffer), src_file);
		if(std::fwrite(buffer, 1, read_size, dst_file) != read_size) {
			return false;
		}
	} while(read_size);
	return true;
}

usize File::size() const {
	if(!_file) {
		return 0;
	}
	std::fpos_t pos = {};
	check_c_err(std::fgetpos(_file, &pos));
	check_c_err(std::fseek(_file, 0, SEEK_END));
	auto len = usize(std::ftell(_file));
	check_c_err(std::fsetpos(_file, &pos));
	return len;
}

usize File::remaining() const {
	if(!_file) {
		return 0;
	}
	std::fpos_t pos = {};
	check_c_err(std::fgetpos(_file, &pos));
	auto offset = usize(std::ftell(_file));
	check_c_err(std::fseek(_file, 0, SEEK_END));
	auto len = usize(std::ftell(_file));
	check_c_err(std::fsetpos(_file, &pos));
	return len - offset;
}

bool File::is_open() const {
	return _file;
}

bool File::at_end() const {
	return _file ? (std::feof(_file) || !remaining()) : true;
}

void File::seek(usize byte){
	if(_file) {
		check_c_err(std::fseek(_file, byte, SEEK_SET));
	}
}

usize File::read(void* data, usize bytes) {
	if(!_file) {
		file_not_opened();
	}
	return std::fread(data, 1, bytes, _file);
}

void File::read_all(core::Vector<u8>& data) {
	usize left = remaining();
	data = core::Vector<u8>(left, 0);
	check_len(read(data.begin(), left), left);
}

void File::write(const void* data, usize bytes) {
	if(!_file) {
		file_not_opened();
	}
	check_len(std::fwrite(data, 1, bytes, _file), bytes);
}

void File::flush() {
	if(_file) {
		check_c_err(std::fflush(_file));
	}
}


}
}

