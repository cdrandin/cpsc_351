#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "ProcessInfo.h"

inline const void MemFail();

int main(int argc, char const *argv[])
{	
	smallint num_process      = 0;
	mediumint memory_size     = 0;
	const mediumint page_sizes[3] = {100, 200, 400};
	mediumint page_size       = 0;

	char* file_name           = NULL;
	std::ifstream* infile     = NULL;
	std::ofstream* outfile    = NULL;
	ProcessInfo* process_info = NULL;

	system("clear");

	// Ask for memory size
	std::cout << "Enter memory size(kbytes)> ";
	std::cin >> memory_size;

	char page_i;
	for(char i=0; i==0;)
	{
		std::cout << "Page Size (1: 100, 2: 200, 3: 400)> ";
		std::cin >> page_i;
		if(page_i < 0 && page_i > 4)
		{
			puts("Choose the options provided");
		}
		else
		{
			i = 1; // stop it
		}
	}

	// Assigned the appropriate page size
	page_size = page_sizes[(int)page_i - 1];

	file_name = new char[10];
	if(file_name == NULL)
	{
		MemFail();
	}

	// Get workload file
	std::cout << "Enter the file name: ";
	std::cin.ignore();
	std::cin.getline(file_name, 10);

	infile = new std::ifstream(file_name);
	if(infile == NULL)
	{
		MemFail();
	}

	if(infile->is_open())
	{
		*infile >> num_process;

		// Create classes for the number of process information we need
		process_info = new ProcessInfo[num_process];

		// File process id
		smallint processID     = 0;
		mediumint arrivalTime  = 0;
		mediumint durationTime = 0;
		mediumint memoryAmount = 0;
		smallint mem_count     = 0;
		mediumint mem_cur      = 0;

		std::cout << num_process << std::endl;
		for(int i = 0;i < num_process;++i)
		{
			*infile >> processID >> arrivalTime >> durationTime >> mem_count;

			process_info[i].SetProcessID(processID);
			process_info[i].SetArrivalTime(arrivalTime);
			process_info[i].SetDurationTime(durationTime);

			for(int j = 0;j<mem_count;++j)
			{
				*infile >> mem_cur;
				memoryAmount += mem_cur;
			}

			process_info[i].SetMemorySize(memoryAmount);

			i = num_process; // just to run it once

			std::cout << i << " " <<num_process<< " information:" << process_info[i].GetProcessID() << " " << arrivalTime << std::endl;
		}
		infile->close();
		delete   infile;
	}
	else
	{
		puts("Unable to open file");
	}

	/*
	// Write to file
	outfile = new std::ofstream("out.txt");
	if(outfile == NULL) 
	{
		MemFail();
	}

	if(outfile->is_open())
	{
		*outfile << "t=0";

		outfile->close();
		delete	 outfile;
	}
	*/

	delete[] file_name;	
	
	if(process_info != NULL)
	{	
		delete[] process_info;
	}

	return 0;
}

inline const void MemFail()
{
	puts("Error: memory could not be allocated");
	exit(-1);	
}