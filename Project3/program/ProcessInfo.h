#include <iostream>
#include <fstream>

#ifndef PROCESSINFO_H
#define PROCESSINFO_H

class ProcessInfo 
{
	private:
		int _processID;
		int _arrivalTime;
		int _durationTime;
		int _memorySize;
	public:
		ProcessInfo(int processID, int arrivalTime, int durationTime, int memorySize);
		inline void nothing(){};
};
#endif