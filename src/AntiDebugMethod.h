#pragma once

#include <windows.h>
#include <iostream>
#include <vector>

class AntiDebugMethod
{
private:
	static int current_id; 
	int id;
	bool detected = false;
	HWND enableButtonHwnd;
public:
	int windowPosX, windowPosY;
	std::string name;
	std::string updated_name;
	bool enabled = false;
	bool (*funcPtr)();
	static void toggleThisMethod(int id);
	static void mainLoop();
	static bool anyDetection;
	static AntiDebugMethod* getMethodById(int id);


	AntiDebugMethod(bool (*funcPtrParam)(), int windowPosXParam, int windowPosYParam, std::string nameParam);

	void toggle();
	bool checkIfDetected();
	bool createGUI(HWND hWnd);
	static std::vector<AntiDebugMethod*> allInstances;
};

