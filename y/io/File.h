/*******************************
Copyright (c) 2016-2019 Grégoire Angerand

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
#ifndef Y_IO_FILE_H
#define Y_IO_FILE_H

#include <y/core/String.h>
#include <y/core/Result.h>

#include "Reader.h"
#include "Writer.h"
#include "Ref.h"

namespace y {
namespace io {

class File final : public Reader, public Writer {

	public:
		File() = default;
		~File() override;

		File(File&& other);
		File& operator=(File&& other);

		static core::Result<File> create(const core::String&  name);
		static core::Result<File> open(const core::String&  name);
		static bool exists(const core::String& name);
		static bool copy(const core::String& src, const core::String& dst);
		static bool copy(io::ReaderRef src, const core::String& dst);

		usize size() const;
		usize remaining() const;

		bool is_open() const;
		bool at_end() const override;

		void seek(usize byte);

		usize read(void* data, usize bytes) override;
		void read_all(core::Vector<u8>& data) override;

		void write(const void* data, usize bytes) override;
		void flush() override;

	private:
		File(std::FILE* f);
		void swap(File& other);

		std::FILE* _file = nullptr;
};

}
}

#endif // Y_IO_FILE_H
