#pragma once
class FlyBuffer
{
public:
	FlyBuffer(int size,int bufferLen,bool isRemove);
	~FlyBuffer(void);
	void* poll();
	void push(void *buffer);
	void clear();
private:
	void *dataQue;
	int size;
	int bufferLen;	
};

