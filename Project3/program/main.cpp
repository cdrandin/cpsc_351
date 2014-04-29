#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "ProcessInfo.h"

int main(int argc, char const *argv[])
{	
	int num_process;
	mediumint memory_size;
	const mediumint page_sizes[3] = {100, 200, 400};
	mediumint page_size;

	char* file_name;
	std::ifstream* infile;
	std::ofstream* outfile;
	ProcessInfo* process_info;

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

	// Get workload file
	std::cout << "Enter the file name: ";
	std::cin.ignore();
	std::cin.getline(file_name, 10);

	infile = new std::ifstream(file_name);

	if(infile->is_open())
	{
		*infile >> num_process;

		std::cout << num_process << std::endl;
		infile->close();
	}
	else
	{
		puts("Unable to open file");
	}

	// Write to file
	outfile = new std::ofstream("out.txt");

	if(outfile == NULL) {
		MemFail();
	}

	if(outfile->is_open())
	{
		*outfile << "t=0";

		outfile->close();
	}
/*
	int* page_sizes;
	uint* memory_size;
	char* file_name;
	std::ifstream* infile;
	std::ofstream* outfile;
	int* num_process;

	page_sizes = new int[3];

	if(page_sizes == NULL)
	{
		MemFail();
	}

	// Constant page numbers
	page_sizes[0] = 100;
	page_sizes[1] = 200;
	page_sizes[2] = 400;

	memory_size = new uint;
	
	if(memory_size == NULL)
	{
		MemFail();
	}

    system("clear");

	// Ask for memory size
	std::cout << "Enter memory size(kbytes)> ";
	std::cin >> *memory_size;

	// Ask for page size. Keep asking till correct selection		
	short* page_i = new short;
	if(page_i == NULL)
	{
		MemFail();
	}

	for(int i=0; i==0;)
	{
		std::cout << "Page Size (1: 100, 2: 200, 3: 400)> ";
		std::cin >> *page_i;
		if(*page_i < 0 && *page_i > 4)
		{
			puts("Choose the options provided");
		}
		else
		{
			i = 1; // stop it
		}
	}

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
		num_process = new int;
		*infile >> *num_process;

		std::cout << *num_process << std::endl;
		infile->close();

		//delete   num_process;
	}
	else
	{
		puts("Unable to open file");
	}

	// Write to file
	outfile = new std::ofstream("out.txt");

	if(outfile == NULL) {
		MemFail();
	}

	if(outfile->is_open())
	{
		*outfile << "t=0";

		outfile->close();
	}


	delete[] page_sizes;
	delete   memory_size;
	delete   page_i;
	delete[] file_name;
	delete   infile;
	delete	 outfile;
	*/

	return 0;
}