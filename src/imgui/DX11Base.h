#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")

namespace dx11_base
{
	bool create(HWND hwnd);
	void frame(void(*on_render)(), bool lock_parent);
	void handle_resize(LPARAM lp);
	void on_render();

	inline std::string new_message;
	inline bool update_message(const char* msg)
	{
		new_message = msg;
		return true;
	}
}