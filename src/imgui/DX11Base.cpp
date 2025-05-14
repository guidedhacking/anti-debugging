#include "DX11Base.h"
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../AntiDebugMethod.h"
#include "../shared.h"

// globals
IDXGISwapChain* swapchain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
ID3D11RenderTargetView* target = nullptr;
FLOAT clear_color[4] = { 34.0935 / 255.f, 34.0935 / 255.f, 34.0935 / 255.f, 1.0f };
ID3D11ShaderResourceView* gh_banner = nullptr;
int gh_banner_size_x, gh_banner_size_y;
float image_scale = 0.343;
int resize_x, resize_y;
HWND window;



std::vector<std::string> get_string_lines(std::string& str);

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-directx11-users
bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	if (out_width)
		*out_width = image_width;
	
	if (out_height)
		*out_height = image_height;
	
	stbi_image_free(image_data);
	return true;
}

bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	FILE* f = nullptr;
	fopen_s(&f, file_name, "rb");
	if (f == NULL)
		return false;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return false;
	fseek(f, 0, SEEK_SET);
	void* file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);
	fclose(f);
	bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
	IM_FREE(file_data);
	return ret;
}

template <typename T> void release(T& t)
{
	t->Release();
	t = nullptr;
}

void style_imgui();

void create_render_target()
{
	ID3D11Texture2D* tx {};
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tx);
	device->CreateRenderTargetView(tx, nullptr, &target);
	release(tx);
}

void clear_render_target()
{
	release(target);
}

bool dx11_base::create(HWND hwnd)
{
	window = hwnd;

	DXGI_SWAP_CHAIN_DESC swap_desc {};
	swap_desc.BufferCount = 2;
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_desc.OutputWindow = window;
	swap_desc.SampleDesc.Count = 1;
	swap_desc.Windowed = TRUE;
	swap_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL output_level {};
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&swap_desc, &swapchain, &device, &output_level, &context
	);

	if (result == S_OK)
	{
		create_render_target();

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(device, context);

		style_imgui();
		LoadTextureFromFile("resources/gh_banner.png", &gh_banner, &gh_banner_size_x, &gh_banner_size_y);

		auto& imGuiIO = ImGui::GetIO();
		imGuiIO.IniFilename = NULL;
		imGuiIO.LogFilename = NULL;

		return true;
	}
	else
	{
		printf("D3D11CreateDeviceAndSwapChain failed: 0x%x\n", result);
		return false;
	}
}

void dx11_base::frame(void(*on_render)(), bool lock_parent)
{
	if (resize_x || resize_y)
	{
		clear_render_target();
		swapchain->ResizeBuffers(0, resize_x, resize_y, DXGI_FORMAT_UNKNOWN, 0);
		resize_x = resize_y = 0;
		create_render_target();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (on_render)
	{
		// lock to bounds of owning window
		if (lock_parent)
		{
			RECT client_rect;
			GetClientRect(window, &client_rect);

			ImGui::SetNextWindowPos(ImVec2(client_rect.left, client_rect.top));
			ImGui::SetNextWindowSize(ImVec2(client_rect.right - client_rect.left, client_rect.bottom - client_rect.top));
		}

		auto& style = ImGui::GetStyle();
		style.WindowBorderSize = 0;
		style.FrameBorderSize = 1;
		on_render();
	}

	ImGui::Render();
	context->OMSetRenderTargets(1, &target, nullptr);
	context->ClearRenderTargetView(target, clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapchain->Present(1, 0); 
}

void dx11_base::handle_resize(LPARAM lp)
{
	resize_x = LOWORD(lp);
	resize_y = HIWORD(lp);
}

void dx11_base::on_render()
{
	ImGui::Begin("###guidedhacking-antidebug", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration);
	
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetCursorPos({ 0, ImGui::GetCursorPosY() });
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Disable ImGui"))
			{
				SendMessageA(window, WM_COMMAND, WM_COMMAND_MENU_DISABLE_IMGUI, NULL);
			}

			if (ImGui::MenuItem("Exit"))
			{
				SendMessageA(window, WM_COMMAND, WM_COMMAND_MENU_ID_EXIT, NULL);
			}				
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("About"))
		{
			SendMessageA(window, WM_COMMAND, WM_COMMAND_MENU_ID_ABOUT, NULL);	
		}
		if (ImGui::MenuItem("Help"))
		{
			SendMessageA(window, WM_COMMAND, WM_COMMAND_MENU_HELP, NULL);
		}
		ImGui::EndMenuBar();
	}

	ImGui::SetCursorPos({ -1, 20 });
	ImGui::Image((ImTextureID)gh_banner, ImVec2(gh_banner_size_x * image_scale, gh_banner_size_y * image_scale));
		
	ImVec2 new_pos = { ImGui::GetWindowWidth() / 2, 80 };
	new_pos.x -= ImGui::CalcTextSize(new_message.c_str()).x * .5;
	new_pos.y -= ImGui::CalcTextSize(new_message.c_str()).y * .45;
	ImGui::SetCursorPos(new_pos);

	if (strstr(new_message.c_str(), "Busted"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 0, 0, 255).Value);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0, 255, 0, 255).Value);
	}

	ImGui::TextWrapped("%s\n", new_message.c_str());
	ImGui::PopStyleColor();

	ImVec2 button_size(230, 50);
	for (auto& method : AntiDebugMethod::allInstances)
	{
		std::string label = method->name;

		if (!method->enabled)
		{
			label += "\nDISABLED";
		}
		else if (method->updated_name.length())
		{
			label = method->updated_name;
		}
		else
		{
			label += "\nENABLED";
		}

		std::string invisible_id = "###btn_" + label;
		ImGui::SetCursorPos(ImVec2(method->windowPosX, method->windowPosY));
		if (ImGui::InvisibleButton(invisible_id.c_str(), button_size))
		{
			method->enabled = !method->enabled;
		}

		// imgui does not support button multiline centering by default
		ImVec2 pos = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImU32 col = ImGui::GetColorU32((ImGui::IsItemClicked() && ImGui::IsItemHovered()) ? ImGuiCol_ButtonActive : ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::GetWindowDrawList()->AddRectFilled(pos, max, col, 0);

		auto lines = get_string_lines(label);
		float max_height = ImGui::GetFont()->FontSize * lines.size();
		float start_y = pos.y + (button_size.y - max_height) / 2.0f;

		for (const auto& line : lines)
		{
			ImVec2 text_size = ImGui::CalcTextSize(line.c_str());
			float x_offset = pos.x + (button_size.x - text_size.x) / 2.0f;
			ImGui::GetWindowDrawList()->AddText(ImVec2(x_offset, start_y), IM_COL32_WHITE, line.c_str());
			start_y += text_size.y;
		}
	}


	ImGui::End();
}

void style_imgui()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

std::vector<std::string> get_string_lines(std::string& str)
{
	std::stringstream ss(str);
	std::string line;
	std::vector<std::string> lines;

	while (std::getline(ss, line, '\n'))
	{
		lines.push_back(line);
	}

	return lines;
}