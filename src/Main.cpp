#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>
#include <chrono>
#include <vector>

DWORD process_id;
DWORD process_base_address;

DWORD XOR_ADDRESS = 0x570458;
DWORD PLAYER_HEALTH_ADDRESS = 0x70E000;
DWORD PLAYER_MANA_ADDRESS = 0x57048C;


int xor_key = 0;
int player_health = 0;
int player_mana = 0;


DWORD getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	DWORD modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (DWORD)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}


int main()
{
	HWND hWnd = FindWindowA(NULL, ("Tibia - Jan Palony"));
	if (hWnd == NULL)
	{
		std::cout << "Could not find the program! \n";
		system("Pause");
		return 0;
	}

	GetWindowThreadProcessId(hWnd, &process_id);
	if (process_id == NULL)
	{
		std::cout << "Could not obtain process ID! \n";
		system("Pause");
		return 0;
	}

	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (!pHandle)
	{
		std::cout << "Could not obtain process handle! \n";
		system("Pause");
		return 0;
	}

	process_base_address = getModuleBaseAddress(process_id, L"Qt5Widgets.dll");

	//UINT_PTR addr = (UINT_PTR)GetModuleHandle(L"Qt5Widgets.dll"); //+ 0x41BE74 + 0x28 + 0x10 + 0x0 + 0x1AC + 0x40 + 0x44;
	//ReadProcessMemory(pHandle, (void*)addr, &player_mana, sizeof(player_mana), 0);

	/*ReadProcessMemory(pHandle, (LPVOID)(XOR_ADDRESS + process_base_address), &xor_key, sizeof(xor_key), 0);
	ReadProcessMemory(pHandle, (LPVOID)(PLAYER_HEALTH_ADDRESS + process_base_address), &player_health, sizeof(player_health), 0);
	ReadProcessMemory(pHandle, (LPVOID)(PLAYER_MANA_ADDRESS + process_base_address), &player_mana, sizeof(player_mana), 0);*/

	/*SendMessage(hWnd, WM_KEYDOWN, 0x39, 0);
	SendMessage(hWnd, WM_KEYUP, 0x39, 0);*/


	//std::cout << process_base_address << "\n";
	/*std::cout << (player_health ^ xor_key) << "\n";*/

	
	/*DWORD offset1, offset2, offset3, offset4, offset5, offset6, offset7;
	ReadProcessMemory(pHandle, (LPCVOID)(process_base_address + 0x0041BE74), &offset1, sizeof(offset1), NULL);
	std::cout << "Offset1: " << std::hex << offset1 << "\n";
	ReadProcessMemory(pHandle, (LPCVOID)(offset1 + 0x28), &offset2, sizeof(offset2), NULL);
	std::cout << "Offset2: " << std::hex << offset2 << "\n";
	ReadProcessMemory(pHandle, (LPCVOID)(offset2 + 0x10), &offset3, sizeof(offset3), NULL);
	std::cout << "Offset3: " << std::hex << offset3 << "\n";
	ReadProcessMemory(pHandle, (LPCVOID)(offset3 + 0x0), &offset4, sizeof(offset4), NULL);
	std::cout << "Offset4: " << std::hex << offset4 << "\n";
	ReadProcessMemory(pHandle, (LPCVOID)(offset4 + 0x1AC), &offset5, sizeof(offset5), NULL);
	std::cout << "Offset5: " << std::hex << offset5 << "\n";
	ReadProcessMemory(pHandle, (LPCVOID)(offset5 + 0x40), &offset6, sizeof(offset6), NULL);
	std::cout << "Offset6: " << std::hex << offset6 << "\n";
	PLAYER_MANA_ADDRESS = offset6 + 0x44;*/

	/* Finding mana adress */
	std::vector<DWORD> mana_offsets = { 0x0041BE74, 0x28, 0x10, 0x0, 0x1AC, 0x40 };
	ReadProcessMemory(pHandle, (LPCVOID)(process_base_address + 0x0041BE74), &mana_offsets[0], sizeof(DWORD), NULL);
	for (size_t i = 1; i < mana_offsets.size(); i++)
	{
		ReadProcessMemory(pHandle, (LPCVOID)(mana_offsets[i-1] + mana_offsets[i]), &mana_offsets[i], sizeof(DWORD), NULL);
	}
	PLAYER_MANA_ADDRESS = mana_offsets[mana_offsets.size() - 1] + 0x44;

	/* Finding health adress */
	std::vector<DWORD> health_offsets = { 0x0041BE74, 0x28, 0x10, 0x0, 0x1AC, 0x40 };
	ReadProcessMemory(pHandle, (LPCVOID)(process_base_address + 0x0041BE74), &health_offsets[0], sizeof(DWORD), NULL);
	for (size_t i = 1; i < health_offsets.size(); i++)
	{
		ReadProcessMemory(pHandle, (LPCVOID)(health_offsets[i - 1] + health_offsets[i]), &health_offsets[i], sizeof(DWORD), NULL);
	}
	PLAYER_HEALTH_ADDRESS = health_offsets[health_offsets.size() - 1] + 0x3C;

	while (true)
	{
		ReadProcessMemory(pHandle, (LPVOID)(PLAYER_MANA_ADDRESS), &player_mana, sizeof(player_mana), NULL);
		ReadProcessMemory(pHandle, (LPVOID)(PLAYER_HEALTH_ADDRESS), &player_health, sizeof(player_health), NULL);
		std::dec;

		std::cout << "Player mana: " << player_mana << "\n";
		std::cout << "Player health: " << player_health << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}

	CloseHandle(pHandle);
	system("Pause");
	return 0;
}

