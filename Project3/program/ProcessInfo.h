#include <iostream>
#include <fstream>

#ifndef PROCESSINFO_H
#define PROCESSINFO_H

typedef unsigned char    smallint; // 0 to 255
typedef unsigned short   mediumint; // 0 to 65,535
typedef unsigned long    largeint; // 0 to 4,294,967,295

class ProcessInfo 
{
	private:
		smallint  _processID;
		mediumint _arrivalTime;
		mediumint _durationTime;
		mediumint _memorySize;
		smallint  _page_size;
		
	public:
		ProcessInfo();
		ProcessInfo(const smallint& processID, 
					const mediumint& arrivalTime, 
					const mediumint& durationTime, 
					const mediumint& memorySize, 
					const smallint& _page_size);
};
#endif