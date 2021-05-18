#pragma once
#pragma comment(lib, "ws2_32")

#ifndef IOSTRUCT_H
#define IOSTRUCT_H

#include <winsock2.h>
#include <iostream>
#include <array>
#include "Stream.h"


enum IOOperation
{
	IO_RECV = 0,
	IO_SEND = 1,
	IO_ERROR
};

struct IoData
{
private:
	OVERLAPPED		overlapped_;
	IOOperation		ioType_;
	size_t totalBytes_;
	size_t currentBytes_;

public:
	std::array<char, SOCKBUF_SIZE> buffer_;

	IoData(IOOperation type) : ioType_(type)
	{
		ZeroMemory(&overlapped_, sizeof(overlapped_));
		this->clear();
	}

	~IoData() {}

	IOOperation& Type() { return ioType_; }

	void clear()
	{
		buffer_.fill(0);
		totalBytes_ = 0;
		currentBytes_ = 0;
	}


	WSABUF SetData(Stream& stream)
	{
		size_t sendSize = stream.size() + 4;
		memcpy_s(buffer_.data(), buffer_.max_size(), stream.data(), (size_t)stream.size());

		WSABUF wsabuf;
		wsabuf.buf = buffer_.data();
		wsabuf.len = sendSize;

		return wsabuf;
	}

	LPWSAOVERLAPPED overlapped() { return &overlapped_; }

	void setType(IOOperation type) { ioType_ = type; }

	char* Data() { return buffer_.data(); }

	WSABUF WsaBuf()
	{
		WSABUF wsabuf;
		wsabuf.buf = buffer_.data();
		wsabuf.len = SOCKBUF_SIZE;

		return wsabuf;
	}

	WSABUF SendSet(int dataSize, char* pMsg) // new로 매번 새로 생성할지 어떻게 할지 고민된다도르
	{
		ZeroMemory(this, sizeof(IoData));
		ioType_ = IO_SEND;
		WSABUF wsabuf;
		wsabuf.len = dataSize;
		wsabuf.buf = buffer_.data();
		std::copy_n(pMsg, dataSize, wsabuf.buf);

		return wsabuf;
	}

};

#endif // !IOSTRUCT_H
