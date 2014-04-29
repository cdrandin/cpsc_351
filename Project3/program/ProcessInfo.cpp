#include "ProcessInfo.h"

ProcessInfo::ProcessInfo():
_processID(0),
_arrivalTime(0), 
_durationTime(0), 
_memorySize(0)
{}

ProcessInfo::ProcessInfo(const int& processID, 
						 const int& arrivalTime, 
						 const int& durationTime, 
						 const int& memorySize, 
						 const int& departureTime,
						 const int& _pageSize):
// initilzer list
_processID(processID),
_arrivalTime(arrivalTime), 
_durationTime(durationTime), 
_memorySize(memorySize),
_departureTime(departureTime)
{}

void ProcessInfo::SetProcessID(const int& processID)
{
	_processID = processID;		
}

void ProcessInfo::SetPageSize(const int& pageSize)
{
	_pageSize = pageSize;
}

void ProcessInfo::SetArrivalTime(const int& arrivalTime)
{
	_arrivalTime = arrivalTime;		
}

void ProcessInfo::SetDurationTime(const int& durationTime)
{
	_durationTime = durationTime;		
}

void ProcessInfo::SetMemorySize(const int& memorySize)
{
	_memorySize = memorySize;		
}
void ProcessInfo::SetDepartureTime(const int& departureTime)
{
	_departureTime = departureTime;		
}

const int ProcessInfo::GetProcessID() const
{
	return _processID;		
}

const int ProcessInfo::GetArrivalTime() const
{
	return _arrivalTime;		
}

const int ProcessInfo::GetDurationTime() const
{
	return _durationTime;		
}

const int ProcessInfo::GetDepartureTime() const
{
	return _departureTime;		
}