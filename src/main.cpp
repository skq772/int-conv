#include <stdio.h>
//#include <string>
#include "imgui.h"
//#include "imgui_stdlib.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "stdvar.h"
#include "convert.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// the three types of conversions for ints
enum IntConvType
{
	SignMagnitude,
	OnesComplement,
	TwosComplement
};

// display int bits with tooltips
void display_int_bits(u64* number, u8 size, IntConvType conv_type)
{
	char* result;
	switch (conv_type)
	{
		case IntConvType::SignMagnitude:
			result = convert::integerToSMBinary(*number, size);
			break;
		case IntConvType::OnesComplement:
			result = convert::integerToOCBinary(*number, size);
			break;
		case IntConvType::TwosComplement:
			result = convert::integerToTCBinary(*number, size);
			break;
	}
	if (result == 0)
	{
		ImGui::Text("--");
		return;
	}

	ImGui::BeginGroup();
	for (u8 i = 0; i < size; i++)
	{
		ImGui::Text("%c", result[i]);

		// toggle bit on click
		if (ImGui::IsItemClicked())
		{
			if (i == 0)
				// special case for the sign bit: invert the number
			 	*number = 1 + ~(*number);
			else
				*number ^= 1ull << (size - i - 1);
		}

		// we need to handle tooltips ourselves because we want them to show up with no delay
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone))
		{
			if (i == 0)
				ImGui::SetTooltip("Sign: %s", result[0] == '1' ? "negative" : "positive");
			else
				ImGui::SetTooltip("%llu", 1ull << (size - i - 1));
		}

		if (i < size-1)
			ImGui::SameLine(0, 0);
	}
	delete[] result;
	ImGui::EndGroup();
}

int main(int, char**)
{
    // this is straight from the example code lol
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(300, 200, "int-conv", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	u64 step_one = 1;

	u64 input = 0;

	u8 int_size = 8;

	// the width of a 0 which should be wider than a 1,
	// this is used to check if we need the wider window size.
	// seems it needs to be called after NewFrame() so i init it to 1
	// (to avoid division by zero later)
	float zero_width = 1;
	
	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glfwSetWindowSize(window, (int_size+1 > (300/zero_width)) ? 470 : 300, 200);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		if (zero_width == 1) zero_width = ImGui::CalcTextSize("0").x;

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		{
			ImGui::Begin(
				"main window", NULL,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar
			);
			
			ImGui::SetNextItemWidth(200);
			ImGui::InputScalar("##", ImGuiDataType_S64, &input, &step_one);

			{
				ImGui::Text("%.3fms %.1fFPS", 1000.0f / io.Framerate, io.Framerate);

				for (u8 byte_i = 0; byte_i < 8; byte_i++)
				{
					u8 byte = input >> ((byte_i^3) * 8);
					// lol
					ImGui::Text(
						"%c%c%c%c%c%c%c%c",
						((byte)&0x80) ? '1' : '0',
						((byte)&0x40) ? '1' : '0',
						((byte)&0x20) ? '1' : '0',
						((byte)&0x10) ? '1' : '0',
						((byte)&0x08) ? '1' : '0',
						((byte)&0x04) ? '1' : '0',
						((byte)&0x02) ? '1' : '0',
						((byte)&0x01) ? '1' : '0'
					);
					if ((byte_i+1) % 4 != 0 && byte_i < 7) ImGui::SameLine();
				}
			}

			if (ImGui::BeginTabBar("MainTabBar"))
			{
				if (ImGui::BeginTabItem("Ints"))
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("size: "); ImGui::SameLine();
					ImGui::SetNextItemWidth(80);
					ImGui::InputScalar("##", ImGuiDataType_U8, &int_size, &step_one);
					if (int_size > 64) int_size = 64;
					if (int_size <  2) int_size = 2;

					ImGui::Text("sign-magn");
					display_int_bits(&input, int_size, IntConvType::SignMagnitude);

					ImGui::Text("1s' compl");
					display_int_bits(&input, int_size, IntConvType::OnesComplement);

					ImGui::Text("2's compl");
					display_int_bits(&input, int_size, IntConvType::TwosComplement);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Floats"))
				{
					ImGui::Text("floats tab!");

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
