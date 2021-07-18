#ifndef _CELL_STREAM_H_
#define _CELL_STREAM_H_
#include <cstdint>
#include <cstring>
class CellStream
{
public:
	CellStream(char* pData, int nSize, bool bDelete = false);
	CellStream(int nsize = 1024);
	~CellStream();

	char* data() { return _pBuff; }
	int length() { return _nWritePos; }
	bool canRead(int n) { return _nsize - _nReadPos >= n; }
	bool canWrite(int n) { return _nsize - _nWritePos >= n; }

	void push(int n) { _nWritePos += n; }
	void pop(int n) { _nReadPos += n; }
	void setWritePos(int n) { _nWritePos = n; }

////read
	template<typename T>
	inline bool Read(T & n, bool bOffset = true)
	{
		auto nLen = sizeof(T);
		if (canRead(nLen)) {
			memcpy(&n, _pBuff + _nWritePos, nLen);
			if (bOffset) {
				pop(nLen);
			}
			return true;
		}
		return false;
	}

	template<typename T>
	inline bool ReadOnly(T & n)
	{
		return Read(n, false);
	}
	template<typename T>
	uint32_t ReadArray(T* pArr, uint32_t len) {
		uint32_t len1 = 0;
		Read(len1, false);
		if (len1 < len) {
			auto nLen = len1 * sizeof(T);
			if(canRead(nLen + sizeof(uint32_t))){
				pop(sizeof(uint32_t));
				memcpy(pArr, _pBuff + _nWritePos, nLen);
				pop(nLen);
				return len1;
			}
		}
		return 0;
	}

	//char size_t
	int8_t ReadInt8(int8_t def = 0);
	//short
	int16_t ReadInt16(int16_t n = 0);
	//int
	int32_t ReadInt32(int32_t n = 0);
	float ReadFloat(float n = 0.0f);
	double ReadDouble(double n = 0.0);

////write
	template<typename T>
	inline bool Write(T & n)
	{
		auto nLen = sizeof(T);
		if (canWrite(nLen)) {
			memcpy(_pBuff + _nWritePos, &n, nLen);
			push(nLen);
			return true;
		}
		return false;
	}

	template<typename T>
	uint32_t WriteArray(T* pArr, uint32_t len) {
		auto nLen = len * sizeof(T);
		if (canWrite(nLen + sizeof(uint32_t))) {
			Write(len);
			memcpy(_pBuff + _nWritePos, pArr, nLen);
			push(nLen);
			return len;
		}
		return 0;
	}

	//char size_t
	int8_t WriteInt8(int8_t def = 0);
	//short
	int16_t WriteInt16(int16_t n = 0);
	//int
	int32_t WriteInt32(int32_t n = 0);
	float WriteFloat(float n = 0.0f);
	double WriteDouble(double n = 0.0);
private:
	//数据缓冲区
	char* _pBuff = nullptr;
	//缓冲区大小
	int _nsize = 0;
	//已写入数据的尾部位置
	int _nWritePos = 0;
	//已读取数据的尾部位置
	int _nReadPos = 0;
	//_pBuff指向外部数据时是否释放
	bool _bDelete = true;

};

#endif // !_CELL_STREAM_H_


