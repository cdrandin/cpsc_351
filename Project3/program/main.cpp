#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

const char* FILE_NAME = "in1.txt";

int main(int argc, char const *argv[])
{	
	int* page_sizes;
	uint* memory_size;
	char* file_name;
	std::ifstream* infile;

	page_sizes = new int[3];

	if(page_sizes == NULL)
	{
		puts("Error: memory could not be allocated");
		exit(-1);
	}

	// Constant page numbers
	page_sizes[0] = 100;
	page_sizes[1] = 200;
	page_sizes[2] = 400;

	memory_size = new uint;
	
    system("clear");

	// Ask for memory size
	std::cout << "Enter memory size(kbytes)> ";
	std::cin >> *memory_size;

	// Ask for page size. Keep asking till correct selection		
	short* page_i = new short;
	for(int i=0;i==0;)
	{
		std::cout << "Page Size (1: 100, 2: 200, 3: 400)> ";
		std::cin >> *page_i;
		if(*page_i < 0 && *page_i > 4)
		{
			puts("Choose the options provided");
		}
		else
		{
			i=1; // stop it
		}
	}

	file_name = new char[10];

	// Get workload file
	std::cout << "Enter the file name: ";
	std::cin >> *file_name;

	infile = new std::ifstream(file_name);

	infile->close();



	delete[] page_sizes;
	delete memory_size;
	delete page_i;
	delete[] file_name;
	delete infile;

	return 0;
}