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
						 const smallint& _page_size):
// initilzer list
_processID(processID),
_arrivalTime(arrivalTime), 
_durationTime(durationTime), 
_memorySize(memorySize)
{}