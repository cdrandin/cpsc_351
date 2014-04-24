#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>

const char* proc_files[] = {
	"/proc/cpuinfo",
	"/proc/version",
	"/proc/meminfo",
	"/proc/uptime",
	"/proc/stat",
	"/proc/diskstats"
};

void StaticProgram();
void DynamicProgram(const char* argv[]);

void GetCPUInfo(float& user_time, float& kernal_time, float& idle_time);
void PrintCPU(const float& user_time, const float& kernal_time, const float& idle_time, const float& last_print);

void GetMemInfo(int& total_memory, int& avaliable_memory);
void PrintMemInfo(const int& total_memory, const int& avaliable_memory);

void GetDiskInfo(int& disk_read_rate, int& disk_read_last, int& disk_write_rate, int& disk_write_last, clock_t& t);
void PrintDiskInfo(int read_rate, int write_rate);

int GetContextRate(int& last, clock_t& t);
void PrintContextSwitches(int rate);

int GetProcessRate(int& last, clock_t& t);
void PrintProcessCreation(int rate);


int Fail();
void FailToOpenFile(const char* file_name);

std::ifstream file_reader;
std::string *file_content = new std::string();	

int main(int argc, const char* argv[])	
{
	if(argc == 1)
	{
		StaticProgram();
	}
	// Dynamic version.
	else if (argc == 3)
	{
		DynamicProgram(argv);
	}

	else
		Fail();

	return 0;
}

void StaticProgram()
{
	int *c = new int(0);

	// Get processor type
	file_reader.open(proc_files[*c]);
	if(file_reader.is_open())
	{
		puts("\nProcessor type: ");
		getline(file_reader, *file_content); // Based on template of the file, this is processor id, so skip
		getline(file_reader, *file_content); // This one should be the processor type (aka. Vendor id)
		std::cout << *file_content << std::endl;
		file_reader.close();
	}
	else
		FailToOpenFile(proc_files[*c]);

	++(*c); // next file

	// Get kernal version
	file_reader.open(proc_files[*c]);
	if(file_reader.is_open())
	{
		puts("\nKernal version: ");
		getline(file_reader, *file_content); // Based on template of the file, this is kernal version
		std::cout << *file_content << std::endl;
		file_reader.close();
	}
	else
		FailToOpenFile(proc_files[*c]);

	++(*c); // next file

	// Get total memory on computer
	file_reader.open(proc_files[*c]);
	if(file_reader.is_open())
	{
		puts("\nTotal memory: ");
		getline(file_reader, *file_content); // Based on template of the file, this is total memory
		std::cout << *file_content << std::endl;
		file_reader.close();
	}
	else
		FailToOpenFile(proc_files[*c]);

	++(*c); // next file

	// Get time since last booted
	file_reader.open(proc_files[*c]);
	if(file_reader.is_open())
	{
		puts("\nTime since last boot: ");
		file_reader >> *file_content; // Based on template of the file, this is time since system was up
		float seconds = atoi(file_content->c_str());
		float minutes = seconds/60.0f;
		float hours   = minutes/60.0f;
		printf("Total seconds: %f or\nTotal minutes: %f or\nTotal hours: %f\n", seconds, minutes, hours);
		file_reader.close();
	}
	else
		FailToOpenFile(proc_files[*c]);

	delete file_content;
	delete c;
}

void DynamicProgram(const char* argv[])
{
	// Get read time
	float read_rate  = atof(argv[1]);
	clock_t read_time;

	// Get print time
	float print_rate = atof(argv[2]);
	clock_t print_time;

	// Record keeping
	float user_time_avg = 0.0f, kernal_time_avg = 0.0f, idle_time_avg = 0.0f, 
		  avaliable_memory_avg = 0.0f,
		  disk_read_rate_avg = 0.0f, disk_write_rate_avg = 0.0f,
		  context_switch_rate_avg = 0.0f,
		  process_creation_rate_avg = 0.0f;

	float user_time = 0.0f, kernal_time = 0.0f, idle_time = 0.0f;
	int total_memory = 0, avaliable_memory = 0; // in kb
	int   disk_read_rate = 0.0f, disk_write_rate = 0.0f,
		  disk_read_last = 0.0f, disk_write_last = 0.0f;

	int context_switch_rate = 0, process_creation_rate = 0;
	int context_switch_last = 0, process_creation_last = 0; // last values to keep track of rate

	float last_print = 0.0f;
	clock_t disk_rate_time;        // Keep track of rate, per second
	clock_t context_switch_time;   // Keep track of rate, per second
	clock_t process_creation_time; // Keep track of rate, per second

	// Not correct numeric values
	if(read_rate <= 0 || print_rate <= 0)
		Fail();

	// Start clock
	print_time = read_time = disk_rate_time = context_switch_time = process_creation_time = clock();

	while(true) 
	{
		context_switch_rate = GetContextRate(context_switch_last, context_switch_time);
		process_creation_rate = GetProcessRate(process_creation_last, process_creation_time);
		GetDiskInfo(disk_read_rate, disk_read_last, disk_write_rate, disk_write_last, disk_rate_time);

		// Update read based on time
		if( ((float)(clock() - read_time)/CLOCKS_PER_SEC) >= read_rate )
		{
			GetCPUInfo(user_time, kernal_time, idle_time);
			GetMemInfo(total_memory, avaliable_memory);
			
			read_time = clock();
			last_print += 1.0f;
		}

		// Update print based on time
		if( ((float)(clock() - print_time)/CLOCKS_PER_SEC) >= print_rate )
		{
			system("clear");

			user_time_avg = user_time - user_time_avg;
			kernal_time_avg = kernal_time - kernal_time_avg;
			idle_time_avg = idle_time - idle_time_avg;

			disk_read_rate_avg = disk_read_rate - disk_read_rate_avg;
			disk_write_rate_avg = disk_write_rate - disk_write_rate_avg;

			context_switch_rate_avg = context_switch_rate - context_switch_rate_avg;

			process_creation_rate_avg = process_creation_rate - process_creation_rate_avg;

			puts("Average");
			PrintCPU(user_time_avg, kernal_time_avg, idle_time_avg, last_print);
			PrintMemInfo(total_memory, avaliable_memory);
			PrintDiskInfo(disk_read_rate_avg, disk_write_rate_avg);
			PrintContextSwitches(context_switch_rate_avg);
			PrintProcessCreation(process_creation_rate_avg);
			puts("");

			print_time   = clock();
			user_time    = 0.0f;
			kernal_time  = 0.0;
			idle_time    = 0.0f;
			last_print   = 0.0f;
		}
	}
}

void GetCPUInfo(float& user_time, float& kernal_time, float& idle_time)
{
	file_reader.open(proc_files[4]);

	file_reader.ignore(3, ' '); // Get cpu
	file_reader.ignore(1, ' '); // ignore space

	// Parse stat file, get key fields we want.
	// - process in user mode(1st field)
	// - process in nice(partial user mode)(2nd mode)
	// - process in kernal mode(3rd field) http://www.linfo.org/kernel_mode.html
	// - process idea (4th field)
	for(int i=0;i<4;++i)
	{
		getline(file_reader, *file_content, ' ');
		
		if(i == 0)
			user_time += atof(file_content->c_str());

		else if(i == 1)
			user_time += atof(file_content->c_str());

		else if(i == 2)
			kernal_time += atof(file_content->c_str());

		else if(i == 3)
			idle_time += atof(file_content->c_str());
	}
	file_reader.close();
}

void PrintCPU(const float& user_time, const float& kernal_time, const float& idle_time, const float& last_print)
{
	float total_cpu_time = user_time + kernal_time + idle_time;
	const float tps = sysconf(_SC_CLK_TCK);

	puts("CPU Info:"); 
	printf("\t%s: %f (%lf%%)\n",   "User mode", (user_time/tps)/ last_print, (user_time/total_cpu_time)*100.0);
	printf("\t%s: %f (%lf%%)\n", "Kernal mode", (kernal_time/tps)/ last_print, (kernal_time/total_cpu_time)*100.0);
	printf("\t%s: %f (%lf%%)\n",   "Idle time", (idle_time/tps)/ last_print, (idle_time/total_cpu_time)*100.0);
}

void GetMemInfo(int& total_memory, int& avaliable_memory)
{
	file_reader.open(proc_files[2]);

	// Total memory
	getline(file_reader, *file_content, '\n');

	// parse memory value
	strtok(const_cast<char*>(file_content->c_str()), " ");
	total_memory = atof(strtok(NULL, " "));

	// Total avaliable memory
	getline(file_reader, *file_content, '\n');

	// parse memory value
	strtok(const_cast<char*>(file_content->c_str()), " ");
	avaliable_memory = atof(strtok(NULL, " "));

	file_reader.close();
}

void PrintMemInfo(const int& total_memory, const int& avaliable_memory)
{
	puts("Memory Info:");
	printf("\t%s: %i kb(%lf%%)\n", "Avaliable memory", avaliable_memory, ((float)avaliable_memory/(float)total_memory)*100.0);
	puts("\tOR");
	printf("\t%s: ~ %i mb(%lf%%)\n", "Avaliable memory", (int)(avaliable_memory*(1.0/1024.0)), ((float)avaliable_memory/(float)total_memory)*100.0);
}

void GetDiskInfo(int& disk_read_rate, int& disk_read_last, int& disk_write_rate, int& disk_write_last, clock_t& t)
{
	file_reader.open(proc_files[5]);
	
	std::string temp;

	if( ((float)(clock() - t)/CLOCKS_PER_SEC) >= 1.0f )
	{
		do
		{
			getline(file_reader, *file_content, '\n');
			strtok(const_cast<char*>(file_content->c_str()), " ");
			strtok(NULL, " ");
			temp = strtok(NULL, " ");

		} while (temp != "sda"); // device name
		
		temp = strtok(NULL, " ");
		temp = strtok(NULL, " ");
		temp = strtok(NULL, " ");
		disk_read_rate = atof(strtok(NULL, " ")); // time spent reading (ms)

		temp = strtok(NULL, " ");
		temp = strtok(NULL, " ");
		temp = strtok(NULL, " ");
		disk_write_rate = atof(strtok(NULL, " ")); // time spent writing (ms)

		disk_read_last = disk_read_rate - disk_read_last;
		disk_read_last = disk_read_rate;

		disk_write_last = disk_write_rate - disk_write_last;
		disk_write_last = disk_write_rate;

		// convert from ms to s
		disk_read_rate /= 1000.0;
		disk_write_rate /= 1000.0;

		t = clock();
	}

	file_reader.close();
}

void PrintDiskInfo(int read_rate, int write_rate)
{
	puts("Disk info:");
	printf("\tRead rate: %i seconds\n", read_rate);
	printf("\tWrite rate: %i seconds\n", write_rate);
}

int GetContextRate(int& last, clock_t& t)
{
	file_reader.open(proc_files[4]);
	int cur;
	int temp;

	if( ((float)(clock() - t)/CLOCKS_PER_SEC) >= 1.0f )
	{
		std::string junk;
		do
		{
			getline(file_reader, *file_content, ' ');
			getline(file_reader, junk, '\n');
		} while (*file_content != "ctxt");
		
		cur = atoi(junk.c_str());
		last = cur - last;

		t = clock();

		temp = last;
		last = cur;
	}

	file_reader.close();
	return std::abs(temp);
}

void PrintContextSwitches(int rate)
{
	puts("Context switch rate:");
	std::cout << "\t" << std::abs(rate) << std::endl;
}

int GetProcessRate(int& last, clock_t& t)
{
	file_reader.open(proc_files[4]);
	int cur;
	int temp;

	if( ((float)(clock() - t)/CLOCKS_PER_SEC) >= 1.0f )
	{
		std::string junk;
		do
		{
			getline(file_reader, *file_content, ' ');
			getline(file_reader, junk, '\n');
		} while (*file_content != "processes");
		
		cur = atoi(junk.c_str());
		last = cur - last;

		t = clock();

		temp = last;
		last = cur;
	}

	file_reader.close();
	return std::abs(temp);
}

void PrintProcessCreation(int rate)
{
	puts("Process creation rate:");
	std::cout << "\t" << std::abs(rate) << std::endl;
}

int Fail()
{
	puts("Error, not correct parameters.\nFormat: \"./proc_parse <read_rate> <printout_rate>\"");
	return -1;
}

void FailToOpenFile(const char* file_name)
{
	printf("Unable to open \"%s file\"\n", file_name);
}