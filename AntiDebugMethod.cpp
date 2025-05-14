#include "AntiDebugMethod.h"
#include <algorithm>

int AntiDebugMethod::current_id = 0; // added
bool AntiDebugMethod::anyDetection = false;
std::vector<AntiDebugMethod*> AntiDebugMethod::allInstances;

void AntiDebugMethod::toggleThisMethod(int id) {
	AntiDebugMethod* methodPtr = AntiDebugMethod::getMethodById(id);
	methodPtr->toggle();
}

AntiDebugMethod* AntiDebugMethod::getMethodById(int id) {
	std::vector<AntiDebugMethod*>::iterator it = std::find_if(AntiDebugMethod::allInstances.begin(), AntiDebugMethod::allInstances.end(), [&id] ( const AntiDebugMethod* method) {
		return id == method->id;
	} );
	if (it != AntiDebugMethod::allInstances.end()) {
		return *it;
	}
	else {
		return nullptr;
	}

}

void AntiDebugMethod::mainLoop() {
	std::vector<AntiDebugMethod*>::iterator ptr;
	AntiDebugMethod::anyDetection = false;
    for (ptr = AntiDebugMethod::allInstances.begin(); ptr < AntiDebugMethod::allInstances.end(); ptr++) {
        
        if ((*ptr)->checkIfDetected() && AntiDebugMethod::anyDetection == false)
        {
        	AntiDebugMethod::anyDetection = true;
	        std::cout << "detected!" << (*ptr)->name << "\n";

        }
    }

}

AntiDebugMethod::AntiDebugMethod(bool (*funcPtrParam)(), int windowPosXParam, int windowPosYParam, std::string nameParam) {
	funcPtr = funcPtrParam;
	windowPosX = windowPosXParam;
	windowPosY = windowPosYParam;
	name = nameParam;
	id = current_id++;
	allInstances.push_back(this);
}


void AntiDebugMethod::toggle() {
	std::cout << "Function toggle() was called \n";
	enabled = !enabled;
	detected = false;
	char newButtonName[200];
	strcpy_s(newButtonName, name.c_str());
	if (enabled)
	{
		strcat_s(newButtonName, "\n ENABLED");
	}
	else {
		strcat_s(newButtonName, "\n DISABLED");
	}

	SendMessageA(enableButtonHwnd, WM_SETTEXT, 0, (LPARAM)newButtonName);
};


bool AntiDebugMethod::checkIfDetected() {
	bool oldDetected = detected;
	if (enabled && funcPtr != nullptr) {
		detected = funcPtr();
		if (oldDetected != detected)
		{
			char newButtonName[200];
			strcpy_s(newButtonName, name.c_str());
			strcat_s(newButtonName, "\n ENABLED - DETECTED!");
			updated_name = newButtonName;
			SendMessageA(enableButtonHwnd, WM_SETTEXT, 0, (LPARAM)newButtonName);
		}
	} else {
		detected = false;
	}
	return detected;
};


bool AntiDebugMethod::createGUI(HWND hWnd) {
	std::cout << "Function createGUI() was called \n";
	char newButtonName[100];
	strcpy_s(newButtonName, name.c_str());
	if (enabled)
	{
		strcat_s(newButtonName, "\n ENABLED");
	}
	else {
		strcat_s(newButtonName, "\n DISABLED");
	}
	enableButtonHwnd = CreateWindowA("button", newButtonName, WS_VISIBLE | WS_CHILD | BS_MULTILINE | SS_CENTER, windowPosX, windowPosY, 230, 50, hWnd, (HMENU)(90+id), NULL,NULL);
	return 1;
};
