#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "Game.h"
#include <thread>

Memory apex_mem;
Memory client_mem;


typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	char name[33] = { 0 };
}player;

struct Matrix
{
	float matrix[16];
};

int main(int argc, char *argv[])
{
	if(geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}

//	const char* cl_proc = "client_ap.exe";
	const char* ap_proc = "ModernWarfare.exe";
	//const char* ap_proc = "EasyAntiCheat_launcher.exe";

	//Client "add" offset
//	uint64_t add_off = 0x3f880;

	while(active)
	{
		if(apex_mem.get_proc_status() != process_status::FOUND_READY)
		{

			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");

			apex_mem.open_proc(ap_proc);

			if(apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("process found\n");
				printf("Base: %lx\n", g_Base);

			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if(client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if(vars_t)
			{
				vars_t = false;
				c_Base = 0;

				vars_thr.~thread();
			}
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");

			client_mem.open_proc(cl_proc);

			if(client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}