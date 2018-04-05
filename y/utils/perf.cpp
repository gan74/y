/*******************************
Copyright (c) 2016-2018 Gr�goire Angerand

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

#include "perf.h"
#include <y/core/Ptr.h>
#include <y/core/Chrono.h>

#include <thread>
#include <mutex>
#include <cstdio>

#include "os.h"

namespace y {
namespace perf {

#ifdef Y_PERF_LOG_ENABLED
static constexpr usize buffer_size = 16 * 1024;
thread_local static std::unique_ptr<char[]> buffer;
thread_local static usize buffer_offset = 0;
thread_local static u32 tid = 0;
static std::mutex mutex;
static bool initialized = false;
static std::unique_ptr<io::Writer> output;

static void write(const char* str, usize len);

static void write_buffer() {
	std::unique_lock lock(mutex);
	if(output && buffer) {
		if(!initialized) {
			initialized = true;
			const char beg[] = R"({"traceEvents":[)";
			output->write(beg, sizeof(beg) - 1);
		}
		output->write(buffer.get(), buffer_offset);
		buffer_offset = 0;
		event("perf", "done writing buffer");
	}
}
#endif

void set_output_ptr(std::unique_ptr<io::Writer>&& out) {
	unused(out);
#ifdef Y_PERF_LOG_ENABLED
	std::unique_lock lock(mutex);
	initialized = false;
	output = std::move(out);
#endif
}

#ifdef Y_PERF_LOG_ENABLED
static void init_thread() {
	if(!buffer.get()) {
		buffer = std::make_unique<char[]>(buffer_size);
		std::stringstream ss;
		ss << std::this_thread::get_id();
		tid = std::stoul(ss.str());
	}
}

static void write(const char* str, usize len) {
	init_thread();
	usize remaining = buffer_size - buffer_offset;
	if(len >= remaining) {
		write_buffer();
	}
	std::memcpy(buffer.get() + buffer_offset, str, len);
	buffer_offset += len;
}

static double micros() {
	return core::Chrono::program().to_micros();
}


static constexpr usize print_buffer_len = 256;

static int paren(const char* buff) {
	if(const char* p = std::strchr(buff, '('); p) {
		return p - buff;
	}
	return print_buffer_len;
}

void enter(const char* cat, const char* func) {
	char b[print_buffer_len];
	usize len = std::snprintf(b, sizeof(b), R"({"name":"%.*s","cat":"%s","ph":"B","pid":0,"tid":%u,"ts":%f},)", paren(func), func, cat, tid, micros());
	if(len >= sizeof(b)) {
		y_fatal("Too long.");
	}
	write(b, len);
}

void leave(const char* cat, const char* func) {
	char b[print_buffer_len];
	usize len = std::snprintf(b, sizeof(b), R"({"name":"%.*s","cat":"%s","ph":"E","pid":0,"tid":%u,"ts":%f},)", paren(func), func, cat, tid, micros());
	if(len >= sizeof(b)) {
		y_fatal("Too long.");
	}
	write(b, len);
}

void event(const char* cat, const char* name) {
	char b[print_buffer_len];
	usize len = std::snprintf(b, sizeof(b), R"({"name":"%s","cat":"%s","ph":"i","pid":0,"tid":%u,"ts":%f},)", name, cat, tid, micros());
	if(len >= sizeof(b)) {
		y_fatal("Too long.");
	}
	write(b, len);
}

static struct Flush : NonCopyable {
	~Flush() {
		char b[print_buffer_len];
		usize len = std::snprintf(b, sizeof(b), R"({"name":"thread closed","cat":"perf","ph":"i","pid":0,"tid":%u,"ts":%f}]})", tid, micros());
		if(len >= sizeof(b)) {
			y_fatal("Too long.");
		}
		write(b, len);
		write_buffer();
	}
} flush;
#endif

}
}
