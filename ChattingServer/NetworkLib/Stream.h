#pragma once

#ifndef STREAM_H
#define STREAM_H

constexpr int SOCKBUF_SIZE = 1024;	//패킷 크기

#include <array>
#include <string>
// #include <windows.h>

class Stream
{
	using UCHAR = unsigned char;
	using str_t = std::string;

private:
	void initialize()
	{
		readPtr_ = 0;
		offset_ = 0;
		stream_.fill(0);
	}

	bool checkWriteBound(size_t len)
	{
		if (offset_ + len > sizeof(stream_))
		{
			printf("socket stream over\n");
			return false;
		}
		return true;
	}

	bool checkReadBound(size_t len)
	{
		if (readPtr_ + len > offset_)
		{
			printf("readOffset : % d, size : % d, totalOffset = % d\n", readPtr_, len, offset_);
			return false;
		}
		return true;
	}

public:
	Stream()
	{
		this->initialize();
	}

	Stream(UCHAR* stream, size_t size)
	{
		this->initialize();
		this->set(stream, size);
	}

	void operator = (Stream& stream)
	{
		this->set(stream.data(), stream.size());
	}

	void set(UCHAR* data, size_t size)
	{
		this->offset_ = size;
		memcpy_s(this->stream_.data(), stream_.size(), (void*)data, size);
	}

	size_t size()
	{
		return offset_;
	}

	UCHAR* data()
	{
		return stream_.data();
	}


	/* WRITE */

	template <typename T>
	void operator << (const T& value)
	{
		// STREAM_WRITE(value);
		size_t size = sizeof(value);
		if (false == this->checkWriteBound(size))
		{
			return;
		}
		memcpy_s((void*)(stream_.data() + offset_), stream_.size() - offset_, (const void*)&value, size);
		offset_ += size;
	}

	void operator << (const str_t value)
	{
		for (auto ch : value)
		{
			*this << ch;
		}
		*this << '\0';
	}

	/* READ */

	void read(void* retVal, size_t len)
	{
		memcpy_s(retVal, len, (void*)(stream_.data() + readPtr_), len);
		readPtr_ += len;
	}


	template <typename T>
	void operator >> (T* retVal)
	{
		size_t size = sizeof(T);
		if (false == this->checkWriteBound(size))
		{
			return;
		}
		this->read((void*)(retVal), size);
	}

	void operator >> (str_t* retVal)
	{
		size_t length = 0;
		while (stream_[readPtr_ + ++length] != '\0') {}
		retVal->clear();
		retVal->assign(&stream_[readPtr_], &stream_[readPtr_ + length]);
		readPtr_ = readPtr_ + length + 1;

	}

private:
	size_t offset_;
	size_t readPtr_;
	std::array<UCHAR, SOCKBUF_SIZE> stream_;


};

#endif
