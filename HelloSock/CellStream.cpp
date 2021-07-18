#include "CellStream.h"

CellStream::CellStream(char * pData, int nSize, bool bDelete)
{
	_pBuff = pData;
	_nsize = nSize;
	_bDelete = bDelete;
}

CellStream::CellStream(int nsize)
{
	_nsize = nsize;
	_pBuff = new char[_nsize];
}

CellStream::~CellStream()
{
	if (_bDelete && _pBuff) {
		delete[] _pBuff;
		_pBuff = nullptr;
	}
}

int8_t CellStream::ReadInt8(int8_t def)
{
	Read(def);
	return def;
}

int16_t CellStream::ReadInt16(int16_t n)
{
	Read(n);
	return n;
}

int32_t CellStream::ReadInt32(int32_t n)
{
	Read(n);
	return n;
}

float CellStream::ReadFloat(float n)
{
	Read(n);
	return n;
}

double CellStream::ReadDouble(double n)
{
	Read(n);
	return n;
}

int8_t CellStream::WriteInt8(int8_t n)
{
	Write(n);
	return n;
}

int16_t CellStream::WriteInt16(int16_t n)
{
	Write(n);
	return n;
}

int32_t CellStream::WriteInt32(int32_t n)
{
	Write(n);
	return n;
}

float CellStream::WriteFloat(float n)
{
	Write(n);
	return n;
}

double CellStream::WriteDouble(double n)
{
	Write(n);
	return n;
}
