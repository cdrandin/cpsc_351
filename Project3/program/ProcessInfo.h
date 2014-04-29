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
		smallint  _pageSize;

		mediumint _arrivalTime;
		mediumint _durationTime;
		mediumint _memorySize;
		
	public:
		ProcessInfo();
		ProcessInfo(const smallint& processID, 
					const mediumint& arrivalTime, 
					const mediumint& durationTime, 
					const mediumint& memorySize, 
					const smallint& _page_size);

		void SetProcessID(const smallint& processID);
		void SetPageSize(const smallint& pageSize);
		
		void SetArrivalTime(const mediumint& arrivalTime);
		void SetDurationTime(const mediumint& durationTime);
		void SetMemorySize(const mediumint& memorySize);
};
#endif