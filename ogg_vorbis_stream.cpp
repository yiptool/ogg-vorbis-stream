/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "ogg_vorbis_stream.h"
#include <yip-imports/cxx-util/macros.h>
#include <yip-imports/cxx-util/istream_util.h>
#include <yip-imports/cxx-util/fmt.h>
#include <iostream>
#include <stdexcept>

OggVorbisStream::OggVorbisStream(std::istream & stream)
	: m_Stream(stream)
{
	ov_callbacks cb;
	cb.read_func = tremorRead;
	cb.seek_func = tremorSeek;
	cb.close_func = tremorClose;
	cb.tell_func = tremorTell;

	int result = ov_open_callbacks(this, &m_Vorbis, nullptr, 0, cb);
	if (UNLIKELY(result < 0))
		throw std::runtime_error(fmt() << "unable to initialize ogg/vorbis decoder (code " << result << ").");
}

OggVorbisStream::~OggVorbisStream()
{
	ov_clear(&m_Vorbis);
}

vorbis_info * OggVorbisStream::info(int link)
{
	vorbis_info * inf = ov_info(&m_Vorbis, link);
	if (UNLIKELY(!inf))
		throw std::runtime_error("ogg/vorbis decoder failed to retrieve stream info.");
	return inf;
}

ogg_int64_t OggVorbisStream::pcmTotal(int link)
{
	ogg_int64_t r = ov_pcm_total(&m_Vorbis, link);
	if (UNLIKELY(r < 0))
	{
		throw std::runtime_error(
			fmt() << "ogg/vorbis decoder failed to retrieve number of PCM samples (code " << r << ").");
	}
	return r;
}

size_t OggVorbisStream::read(void * buf, size_t size)
{
	int stream = 0;
	long r = ov_read(&m_Vorbis, reinterpret_cast<char *>(buf), int(size), &stream);
	if (UNLIKELY(r < 0))
		throw std::runtime_error(fmt() << "ogg/vorbis decoder failed to decode the data (code " << r << ").");
	return size_t(r);
}

void OggVorbisStream::pcmSeek(ogg_int64_t offset)
{
	int r = ov_pcm_seek(&m_Vorbis, offset);
	if (UNLIKELY(r < 0))
		throw std::runtime_error(fmt() << "ogg/vorbis decoder failed to seek (code " << r << ").");
}

void OggVorbisStream::timeSeek(ogg_int64_t offset)
{
	int r = ov_time_seek(&m_Vorbis, offset);
	if (UNLIKELY(r < 0))
		throw std::runtime_error(fmt() << "ogg/vorbis decoder failed to seek (code " << r << ").");
}

size_t OggVorbisStream::tremorRead(void * buf, size_t size, size_t nmemb, void * ud)
{
	OggVorbisStream * self = reinterpret_cast<OggVorbisStream *>(ud);

	try
	{
		return istream_read(self->m_Stream, buf, size);
	}
	catch (const std::exception & e)
	{
		std::clog << "error reading ogg/vorbis data: " << e.what() << std::endl;
		return 0;
	}
}

int OggVorbisStream::tremorSeek(void * ud, ogg_int64_t offset, int whence)
{
	OggVorbisStream * self = reinterpret_cast<OggVorbisStream *>(ud);

	try
	{
		std::ios_base::seekdir dir;
		switch (whence)
		{
		case SEEK_SET: dir = std::ios_base::beg; break;
		case SEEK_CUR: dir = std::ios_base::cur; break;
		case SEEK_END: dir = std::ios_base::end; break;
		default: throw std::runtime_error("invalid whence.");
		}

		istream_seek(self->m_Stream, static_cast<std::streamoff>(offset), dir);

		return 0;
	}
	catch (const std::exception & e)
	{
		std::clog << "error reading ogg/vorbis data: " << e.what() << std::endl;
		return -1;
	}
}

int OggVorbisStream::tremorClose(void *)
{
	return 0;
}

long OggVorbisStream::tremorTell(void * ud)
{
	OggVorbisStream * self = reinterpret_cast<OggVorbisStream *>(ud);

	try
	{
		return static_cast<long>(self->m_Stream.tellg());
	}
	catch (const std::exception & e)
	{
		std::clog << "error reading ogg/vorbis data: " << e.what() << std::endl;
		return -1;
	}
}
