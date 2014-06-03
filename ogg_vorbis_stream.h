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
#ifndef __75896c74fe26319a3a43b17df61b0d83__
#define __75896c74fe26319a3a43b17df61b0d83__

#include <istream>
#include <memory>
#include <yip-imports/ivorbisfile.h>

class OggVorbisStream
{
public:
	OggVorbisStream(std::istream & stream);
	~OggVorbisStream();

	vorbis_info * info(int link = -1);
	ogg_int64_t pcmTotal(int link = -1);

	size_t read(void * buf, size_t size);

private:
	std::istream & m_Stream;
	OggVorbis_File m_Vorbis;

	static size_t tremorRead(void * buf, size_t size, size_t nmemb, void * ud);
	static int tremorSeek(void * ud, ogg_int64_t offset, int whence);
	static int tremorClose(void * ud);
	static long tremorTell(void * ud);

	OggVorbisStream(const OggVorbisStream &);
	OggVorbisStream & operator=(const OggVorbisStream &);
};

typedef std::shared_ptr<OggVorbisStream> OggVorbisStreamPtr;

#endif
