#include <iostream>
#include <fstream>

#ifndef PROCESSINFO_H
#define PROCESSINFO_H

class ProcessInfo 
{
	private:
		int  _processID;
		int  _pageSize;

		int _arrivalTime;
		int _durationTime;
		int _memorySize;
		int _departureTime;
		
	public:
		ProcessInfo();
		ProcessInfo(const int& processID, 
					const int& arrivalTime, 
					const int& durationTime, 
					const int& memorySize,
					const int& _departureTime, 
					const int& _page_size);

		void SetProcessID(const int& processID);
		void SetPageSize(const int& pageSize);
		
		void SetArrivalTime(const int& arrivalTime);
		void SetDurationTime(const int& durationTime);
		void SetMemorySize(const int& memorySize);

		void SetDepartureTime(const int& departureTime);
		
		const int GetProcessID() const;
		const int GetArrivalTime() const;
		const int GetDurationTime() const;
		const int GetDepartureTime() const;

};
#endif