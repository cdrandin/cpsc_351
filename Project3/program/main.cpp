#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <queue>

#include "ProcessInfo.h"

inline const void MemFail();
const void PrintInputQueue(std::queue<int> inputQueue, std::ofstream& outfile);
const void PrintMemoryMap(const int memory_size, const int page_size, std::ofstream& outfile);


int main(int argc, char const *argv[])
{	
	int num_process           = 0;
	int memory_size           = 0;
	const int page_sizes[3] = {100, 200, 400};
	int page_size             = 0;

	char* file_name           = NULL;
	std::ifstream* infile     = NULL;
	ProcessInfo* process_info = NULL;

	system("clear");

	// Ask for memory size
	std::cout << "Enter memory size(kbytes)> ";
	std::cin >> memory_size;

	int page_i;
	for(int i=0; i==0;)
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
	page_size = page_sizes[page_i-1];

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
		int processID    = 0;
		int arrivalTime  = 0;
		int durationTime = 0;
		int memoryAmount = 0;
		int mem_count    = 0;
		int mem_cur      = 0;

		for(int i = 0;i < num_process; ++i)
		{
			*infile >> processID >> arrivalTime >> durationTime >> mem_count;

			process_info[i].SetProcessID(processID);
			process_info[i].SetArrivalTime(arrivalTime);
			process_info[i].SetDurationTime(durationTime);
			process_info[i].SetDepartureTime(-1); // make -1 if not in memory yet

			for(int j = 0;j < mem_count;++j)
			{
				*infile >> mem_cur;
				memoryAmount += mem_cur;
			}

			process_info[i].SetMemorySize(memoryAmount);
			printf("ProcessID: %d  Arrivale Time: %d  Duration Time: %d  Memort Amount: %d\n", processID, arrivalTime, durationTime, memoryAmount);
			
			memoryAmount = 0;
		}

		infile->close();
		delete   infile;
	}
	else
	{
		puts("Unable to open file");
	}



	// Write to file
	std::ofstream outfile;
	outfile.open("out.txt");

	// clock loop
	unsigned int virtualClock = 0;
	std::queue<int> inputQueue;
	int currentProcessID = 0;
	int process_met = 0;

	// write to file
	if(outfile.is_open())
	{
		// clock loop
		for(int i=0; i==0;) // for(process)
		{
			for(int j = 0; j < num_process; ++j)
			{

				std::cout << process_info[j].GetDepartureTime() << " " << virtualClock << std::endl;
				// find process with departure time == virtualClock
				if(process_info[j].GetDepartureTime() == (int)virtualClock)
				{
					std::cout << "got in\n";
					outfile << "t = " << virtualClock << ":  Process " << process_info[j].GetProcessID() << " completes\n";
					
					// remove process from memory
					// =====NEED SHIT HEREEEE

					// another process is done, update counter
					++process_met;

					// Print out memory map
					PrintMemoryMap(memory_size, page_size, outfile);
				}

				// find proceesses with arrival time == virtualClock
				if(process_info[j].GetArrivalTime() == (int)virtualClock)
				{
					outfile << "t = " << virtualClock << ":  Process " << process_info[j].GetProcessID() << " arrives\n";
					// add process to input queue
					inputQueue.push(process_info[j].GetProcessID());
					PrintInputQueue(inputQueue, outfile);
				}
				std::cin.get();
			}

			// Put the first process in queue onto memory
			while (!inputQueue.empty())
			{
				// get and pop first process on queue
				currentProcessID = inputQueue.front();
				inputQueue.pop();
				std::cout << "process ID" << currentProcessID << std::endl;

				// Prints MM moves and queue messages
				outfile << "\tMM moves Process " << currentProcessID << " to memory\n";
				PrintInputQueue(inputQueue, outfile);

				// move process into memory
				// =====NEED SHIT HEREEEE

				// update process departure time
				process_info[currentProcessID-1].SetDepartureTime(process_info[currentProcessID-1].GetDurationTime() + virtualClock);
				std::cout << "Departure Time" << process_info[currentProcessID-1].GetDepartureTime() << std::endl;

				// Prints Memory map
				PrintMemoryMap(memory_size, page_size, outfile);
			}

			// Time increases
			virtualClock += 100;
			std::cout << process_met << std::endl;
			if(process_met > num_process)
				i=1;
		}

		outfile.close();
	}

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

const void PrintInputQueue(std::queue<int> inputQueue, std::ofstream& outfile)
{
	outfile << "\tInput Queue: [";
	if(!inputQueue.empty())
	{
		for(int i=0;i<=(int)inputQueue.size();++i)
		{
			outfile << inputQueue.front();
			if(i < (int)inputQueue.size()-1)
				outfile << " ";
			inputQueue.pop();
		}
	}

	outfile << "]\n";
}

const void PrintMemoryMap(const int memory_size, const int page_size, std::ofstream& outfile)
{
	int mapInc = memory_size/page_size;
	outfile << "\tMemory Map: ";
	// loop for each page of memory map
	for(int i=0; i<mapInc; i++)
	{
		if(i > 0)
			outfile << "\t\t    ";
		outfile << i*page_size << "-" << (i+1)*page_size-1 << ":\n";
	}
}