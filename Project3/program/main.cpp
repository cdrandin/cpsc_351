#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <vector>
#include <cmath>

#include "ProcessInfo.h"

inline const void MemFail();
const void PrintInputQueue(std::queue<int> inputQueue, std::ofstream& outfile);
const void PrintMemoryMap(const int& memory_size, const int& page_size, const int& num_process, const ProcessInfo* process_info, std::ofstream& outfile, int* memory_block);

const int NumberOfFreeFrames(const int* memory_block, const int& size);

int main(int argc, char const *argv[])
{	
	int num_process           = 0;
	int memory_size           = 0;
	const int page_sizes[3] = {100, 200, 400};
	int page_size             = 0;

	char* file_name           = NULL;
	std::ifstream* infile     = NULL;
	ProcessInfo* process_info = NULL;

	int* memory_block         = NULL;

	system("clear");

	// Ask for memory size
	for(int i=0; i==0;++i)
	{
		std::cout << "Enter memory size(kbytes)> ";
		std::cin >> memory_size;
		if(memory_size < 0)
		{
			puts("Enter a positive size");
		}
		else
		{
			i = i; // stop it
		}
	}

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
	bool once = false;

	// write to file
	if(outfile.is_open())
	{
		memory_block = new int[memory_size/page_size];
		// Our way of saying it is free space
		for(int i = 0; i < memory_size/page_size; ++i)
		{
			memory_block[i] = -1;
		}

		// clock loop
		for(int i=0; i==0;) // for(process)
		{
			once = false;

			for(int j = 0; j < num_process; ++j)
			{
				// find proceesses with arrival time == virtualClock
				if(process_info[j].GetArrivalTime() == (int)virtualClock)
				{
					if(!once)
					{
						outfile << "\nt = " << virtualClock << ":  Process " << process_info[j].GetProcessID() << " arrives\n";
						once = true;
					}

					// add process to input queue
					inputQueue.push(process_info[j].GetProcessID());
					PrintInputQueue(inputQueue, outfile);
				}
			}
		
			// Put the first process in queue onto memory
			while (!inputQueue.empty())
			{
				currentProcessID = inputQueue.front();
				if(NumberOfFreeFrames(memory_block, memory_size/page_size) * page_size >= process_info[currentProcessID-1].GetMemorySize())
				{
					// get and pop first process on queue
					inputQueue.pop();

					// Prints MM moves and queue messages
					outfile << "       MM moves Process " << currentProcessID << " to memory\n";
					PrintInputQueue(inputQueue, outfile);

					// move process into memory
					// =====NEED SHIT HEREEEE
					// Checking if the process can fit in the memory block
					
					int numOfPagesNeeded = (int)ceil((double)process_info[currentProcessID-1].GetMemorySize()/(double)page_size);
					int count = 0;

					// FIll up the memory block with the valid processes
					for(int j = 0; j < memory_size/page_size; ++j)
					{
						if(memory_block[j] == -1 && count < numOfPagesNeeded)
						{
							memory_block[j] = process_info[currentProcessID-1].GetProcessID();
							++count;
						}
					}

					// update process departure time
					process_info[currentProcessID-1].SetDepartureTime(process_info[currentProcessID-1].GetDurationTime() + virtualClock);

					// Prints Memory map
					PrintMemoryMap(memory_size, page_size, num_process, process_info, outfile, memory_block);
					}
					else
					{
						break;
					}
			}


			for(int j = 0; j < num_process; ++j)
			{
				// find process with departure time == virtualClock
				if(process_info[j].GetDepartureTime() == (int)virtualClock)
				{
					if(!once)
					{
						outfile << "\nt = " << virtualClock << ":  ";
						once = true;
					}
					else{
						outfile << "       ";
					}
					outfile << "Process " << process_info[j].GetProcessID() << " completes\n";
			
					// remove process from memory
					// =====NEED SHIT HEREEEE
					for(int k = 0; k < memory_size/page_size; ++k)
					{
						if(memory_block[k] == process_info[j].GetProcessID())
						{
							memory_block[k] = -1;
						}
					}

					// another process is done, update counter
					++process_met;

					// Print out memory map
					PrintMemoryMap(memory_size, page_size, num_process, process_info, outfile, memory_block);
				}
			}

			// Time increases
			virtualClock += 100;
			if(process_met == num_process)
				i=1;
		}

		outfile.close();
	}

	delete[] file_name;	
	
	if(process_info != NULL)
	{	
		delete[] process_info;
	}

	if(memory_block != NULL)
	{
		delete[] memory_block;
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
	outfile << "       Input Queue: [";
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

const void PrintMemoryMap(const int& memory_size, const int& page_size, const int& num_process, const ProcessInfo* process_info, std::ofstream& outfile, int* memory_block)
{
	int mapInc           = memory_size/page_size;
	int currentProcessID = 0, 
		currentPage      = 0;
	int oldProcessID	 = 0;
	int start_mem        = 0, 
		end_mem          = 0;
	int inFreeFrame	 	 = -1; // -1: not in frame, 0: last frame, 1: first frame

	outfile << "       Memory Map: ";

	// loop for each page of memory map
	for(int i = 0; i < mapInc; ++i)
	{
		if(i > 0 && inFreeFrame < 0)
		{
			outfile << "                  ";
		}

		// checks which process we are on compare to old process
		currentPage += 1;
		currentProcessID = memory_block[i];

		if (oldProcessID != currentProcessID)
			currentPage =1;

		// check where in free frame we are
		// Notice: it can be first free frame and the last; therefore different if statements
		
		// First free frame: current is free, past is not free
		if (currentProcessID == -1 && oldProcessID >= 0) 
		{
			inFreeFrame = 1;
			start_mem = i*page_size;
		}
		
		// Last free frame: is in free frame and at the end of memory or the next frame not free
		if (inFreeFrame > 0 && (memory_block[i+1] > -1 || i == mapInc-1))
		{
			inFreeFrame = 0;
			end_mem = (i+1)*page_size-1;
			outfile << start_mem << "-" << end_mem << ": Free Frame(s)" << std::endl;
		}

		// not in free frame
		if (inFreeFrame == -1)
		{
			//inFreeFrame = -1;
			start_mem = i*page_size;
			end_mem   = (i+1)*page_size-1;
			outfile << start_mem << "-" << end_mem << ": Process " << currentProcessID<< ", Page " << currentPage << std::endl;
		}

		// resets old process to current for next loop
		oldProcessID = currentProcessID;
		// if the last frame has been printed, set free frame to -1 for next loop
		if (inFreeFrame == 0)
			inFreeFrame = -1;
		

	}
}

const int NumberOfFreeFrames(const int* memory_block, const int& size)
{
	int count = 0;
	for(int i = 0;i < size;++i)
	{
		if(memory_block[i] == -1)
			++count;
	}

	return count;
}