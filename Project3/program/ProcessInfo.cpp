#include "ProcessInfo.h"

ProcessInfo::ProcessInfo():
_processID(0),
_arrivalTime(0), 
_durationTime(0), 
_memorySize(0)
{}

ProcessInfo::ProcessInfo(const smallint& processID, 
						 const mediumint& arrivalTime, 
						 const mediumint& durationTime, 
						 const mediumint& memorySize, 
						 const smallint& _pageSize):
// initilzer list
_processID(processID),
_arrivalTime(arrivalTime), 
_durationTime(durationTime), 
_memorySize(memorySize)
{}

void ProcessInfo::SetProcessID(const smallint& processID)
{
	_processID = processID;		
}

void ProcessInfo::SetPageSize(const smallint& pageSize)
{
	_pageSize = pageSize;
}

void ProcessInfo::SetArrivalTime(const mediumint& arrivalTime)
{
	_arrivalTime = arrivalTime;		
}

void ProcessInfo::SetDurationTime(const mediumint& durationTime)
{
	_durationTime = durationTime;		
}

void ProcessInfo::SetMemorySize(const mediumint& memorySize)
{
	_memorySize = memorySize;		
}

smallint ProcessInfo::GetProcessID()
{
	return _processID;		
}