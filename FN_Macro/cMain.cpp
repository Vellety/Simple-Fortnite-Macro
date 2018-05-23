#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "hCFG-Classes.h"
#include "hSecurity.h"

// bool
bool bRead = false;
bool bExit = false;

// string
std::string sDGValue  = "";
std::string sDGSleep  = "";
std::string sDGSleep2 = "";

// int
int iDGValue  = 0;
int iDGSleep  = 0;
int iDGSleep2 = 0;

// Just to make life a little less of a clusterfuck
void PressKey(int vkey)
{
	keybd_event(vkey, 0, 0, 0);
	keybd_event(vkey, 0, KEYEVENTF_KEYUP, 0);
}

/* entry point */
int main()
{
	// Randomize title
	const auto ch_set = charset();                              // Create character set. 
	std::default_random_engine rng(std::random_device{}());     // Create non-deterministic RNG
	std::uniform_int_distribution<> dist(0, ch_set.size() - 1); // Create random number "shaper" that will give us uniformly distributed indices into the character set

	// Tie the whole thing together :))
	auto randchar = [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; };
	auto sLength = 15;

	random_string(sLength, randchar);

	std::vector<char> cTitle(random_string(sLength, randchar).begin(), random_string(sLength, randchar).end());
	cTitle.push_back('\0');

	SetConsoleTitle(&cTitle[0]);

	// Begin actual app
	std::cout << "[~] Reading cfg file.." << std::endl;
	while (!bRead)
	{
		cfgFile cfg("keybinds.cfg"); // Set which file to read
		if (cfg.keyExists("dg-key") && cfg.keyExists("first-weapon") && cfg.keyExists("second-weapon")) // Check file for valid keys
		{
			// DG
			sDGValue = cfg.getValueOfKey<std::string>("dg-key");         // Get the user value of mentioned key
			StrToIntEx(sDGValue.c_str(), STIF_SUPPORT_HEX, &iDGValue);   // Convert and set read value to specified integer

			// Weapon 1
			sDGSleep = cfg.getValueOfKey<std::string>("first-weapon");   // No need to support hex conversion on these 2
			StrToIntEx(sDGSleep.c_str(), STIF_DEFAULT, &iDGSleep);       //

			// Weapon 2
			sDGSleep2 = cfg.getValueOfKey<std::string>("second-weapon");
			StrToIntEx(sDGSleep2.c_str(), STIF_DEFAULT, &iDGSleep2);
		}
		else // Exit app if file has missing keys
		{
			std::cout << "[-] CFG file was read but one or more valid keys are missing." << std::endl;
			std::cout << "Exiting shortly.." << std::endl;

			Sleep(1500);
			bExit = true;
		}

		bRead = true;
	}

	if (!bExit) // Avoid unintended success msg if read fail
	{
		std::cout << "[+] CFG file was read and valid keys were found!" << std::endl;
		std::cout << "(dg-key=" << iDGValue << ", first-weapon=" << iDGSleep << "ms, second-weapon=" << iDGSleep2 << "ms)\n" << std::endl;
	}

	std::cout << "Macro is now running, have fun! <3" << std::endl; // Announce victory
	std::cout << "Press 'END' key to exit app properly.\n" << std::endl;  // Inform user of proper way to exit the application.
	while (!bExit)
	{
		// Macro 1
		// Double Gun
		if (GetAsyncKeyState(iDGValue))
		{
			// Step 1
			PressKey(0x01);   // Fire
			PressKey(0x34);   // Swap to nr4
			Sleep(iDGSleep2); // Timeout for user-specified amount

			// Step 2
			PressKey(0x01);   // Fire again
			PressKey(0x33);   // Swap to nr3
			Sleep(iDGSleep);  // Timeout for user-specified amount again
		}

		// Set 'bExit' to return 0
		if (GetAsyncKeyState(VK_END))
		{
			printf_s("[~] ");
			printf_s("Exiting..\n");

			Sleep(1000);
			bExit = true;
		}

		Sleep(10); // Reduce that CPU usage by a notch
	}

	return 0;
}
