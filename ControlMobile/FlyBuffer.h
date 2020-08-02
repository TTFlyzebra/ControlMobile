#pragma once
class FlyBuffer
{
public:
	FlyBuffer(int length,int size,bool isRemove);
	~FlyBuffer(void);
	void* poll();
	void push(void *buffer);
	void clear();
private:
	void *dataQue;
	
};

