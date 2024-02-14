#include "imgui.h"
#include "imgui_freetype.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>
#include "stdvar.h"
#include "i18n/i18n.cpp" // ??? doesn't work with .h
#include "convert.h"
#include "terminusttf.h"

// Constants
const f32 font_size = 16.0f; // Real rasterized font size
const u32 window_w = 600;
const u32 window_h = 320;
const u32 color_white = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
const u32 color_light_blue = IM_COL32(0x00, 0xAA, 0xFF, 0xFF);
const u32 color_dark_blue = IM_COL32(0x00, 0x55, 0xFF, 0xFF);

const ImWchar default_plus_polish_glyph_ranges[] = {
	0x0020, 0x00FF,
	0x0104, 0x0107,
	0x0118, 0x0119,
	0x0141, 0x0144,
	0x015A, 0x015B,
	0x0179, 0x017C,
	0
};

// global translator object
I18n::Translator translator;
// shorthand
#define _(t) translator.get_str(t)
// so we can do just id::whatever
using I18n::id;

// Settings
i32 gui_size = 1;							// Size of the entire GUI
bool multibit_invertion = false;			// Invert multiple bits with one click and mouse movement
bool show_bit_weights = true;				// Show the tooltips
bool color_bit_sections = true;				// Color background of binary representation
bool invert_color = false;					// Color background of bit fields
bool show_option_help = true;				// Show tooltip after hovering on a setting
i32 selected_tab = 0;						// Current tab

// Strip file name from the path, copy it to new C string and append the new filename to it.
// If the path or the new filename is a null pointer, zero is returned.
char* replaceFilename(const char* path, const char* new_filename)
{
	if (!path || !new_filename)
		return 0;
	i8 index = 0;
	while (path[index++]);
	while (index--)
		if (path[index] == '\\' || path [index] == '/')
			break;
	i8 new_filename_length = -1;
	while (new_filename[++new_filename_length]);
	char* result = new char[++index + ++new_filename_length];
	while (new_filename_length--)
		result[index + new_filename_length] = new_filename[new_filename_length];
	while (index--)
		result[index] = path[index];
	return result;
}

// Save current settings to file.
// If path is a null pointer, nothing happens.
void saveSettings(const char* path)
{
	FILE* f = fopen(path, "w");
	if (!f)
		return;
	fprintf(f, "%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i\n", 
		(u16)translator.lang,
		gui_size,
		multibit_invertion,
		show_bit_weights,
		color_bit_sections,
		invert_color,
		show_option_help,
		selected_tab);
	fclose(f);
}

// Load variables from file.
// If path is a null pointer or the file doesn't exist or has incorrect values, default settings are loaded.
void loadSettings(const char* path)
{
	u16 tmp_lang = 1;
	std::fstream f(path);
	if(f.is_open())
	{
		f
		>>tmp_lang
		>>gui_size
		>>multibit_invertion
		>>show_bit_weights
		>>color_bit_sections
		>>invert_color
		>>show_option_help
		>>selected_tab;
		f.close();
	}
	if (tmp_lang <= 2)
		translator.lang = (I18n::lang_id)tmp_lang;
	if (gui_size < 1 || gui_size > 20)
		gui_size = 1;
	if (selected_tab < 1 || selected_tab > 5)
		selected_tab = 1;
}

// Print clickable bit chain.
// If the chain is modified, the modified bit index is returned.
// If length is less than 2 or greater than 64, nothing is printed.
i8 print_bits(
	char*& bit_array, 
	u8 length, 
	bool is_color_background, 
	u64 color_pattern, 
	u32 color_default, 
	u32 color_1, 
	u32 color_2, 
	i8 last_bit_inverted, 
	char* (*tooltips)(u8, u8, const char*))
{
	if (!bit_array)
	{
		while (length--)
		{
			ImGui::TextUnformatted("-");
			if (length)
			{
				ImGui::SameLine(0, 0);
				if (!((length)%8))
				{
					ImGui::TextUnformatted(" ");
					ImGui::SameLine(0, 0);
				}
			}
		}
	}
	if (length < 2 || length > 64 || !bit_array)
		return -1;
	i8 result = last_bit_inverted;
	bool is_released = true;
	char* color_pattern_bits = convert::integerToTCBinary(color_pattern, 64);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGui::BeginGroup();
	for (u8 i = 0; bit_array[i]; ++i)
	{
		if (is_color_background)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_default);
			draw_list->AddRectFilled(ImGui::GetCursorScreenPos(), 
			{ImGui::CalcTextSize("0").x + ImGui::GetCursorScreenPos().x, ImGui::CalcTextSize("0").y + ImGui::GetCursorScreenPos().y}, 
			(color_pattern_bits[i] == '0') ? color_1 : color_2);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, (color_pattern_bits[i] == '0') ? color_1 : color_2);
			draw_list->AddRectFilled(
				ImGui::GetCursorScreenPos(), 
				{ImGui::CalcTextSize("0").x + ImGui::GetCursorScreenPos().x, ImGui::CalcTextSize("0").y + ImGui::GetCursorScreenPos().y}, 
				color_default);
		}
		ImGui::Text("%c", bit_array[i]);
		ImGui::PopStyleColor();
		if (
			(ImGui::IsItemClicked() && last_bit_inverted < 0) || 
			(last_bit_inverted > -1 && 
			last_bit_inverted != i && 
			(ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1) || ImGui::IsMouseDown(2)) && 
			ImGui::IsItemHovered()))
		{
			result = i;
			bit_array[i] ^= 1;
		}
		if (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1) || ImGui::IsMouseDown(2))
			is_released = false;
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && tooltips)
		{
			char* tooltip = tooltips(length, i, bit_array);
			if (tooltip)
			{
				ImGui::SetTooltip("%s", tooltip);
				delete tooltip;
			}
		}
		if (bit_array[i + 1])
		{
			ImGui::SameLine(0, 0);
			if (!((length-i-1)%8))
			{
				ImGui::TextUnformatted(" ");
				ImGui::SameLine(0, 0);
			}
		}
	}
	ImGui::EndGroup();
	delete color_pattern_bits;
	if (is_released)
		return -1;
	return result;
}

char* integer_tooltips(u8 count, u8 index, const char* values)
{
	char* buffer = new char[64];
	if (!index)
		sprintf(
			buffer, 
			"%s: %s", 
			_(id::g_sign), 
			(values[index] == '0') ? _(id::g_plus) : _(id::g_minus));
	else
		sprintf(buffer, 
			"%s: %llu", 
			_(id::g_bit_value), 
			1ull<<(count-index-1));
	return buffer;
}

char* float_tooltips(u8 count, u8 index, const char* values)
{
	if (count != 32)
		return 0;
	char* buffer = new char[64];
	if (!index)
		sprintf(
			buffer, 
			"%s: %s", 
			_(id::g_sign), 
			(values[index] == '0') ? _(id::g_plus) : _(id::g_minus));
	else if (index < 9)
	{
		u8 exponent = 0;
		u8 exponent_bit_index = 9;
		while (--exponent_bit_index)
			if (values[exponent_bit_index] == '1')
				exponent += 1<<(8 - exponent_bit_index);
		exponent -= 127;
		sprintf(
			buffer, 
			"%s: %i, %s: %i", 
			_(id::f_exponent), 
			(i8)exponent, 
			_(id::g_bit_value), 
			1<<(8 - index));
	}
	else
	{
		f32 normalized_mantissa = 1.0;
		u8 mantissa_index = 8;
		while (++mantissa_index < 32)
			if (values[mantissa_index] == '1')
				normalized_mantissa += 0.5/(1<<(mantissa_index - 9));
		sprintf(
			buffer, 
			"%s: %f, %s:%s%f", 
			_(id::f_mantissa), 
			normalized_mantissa, 
			_(id::g_bit_value), 
			(1.0/(1<<(index - 8)) < 0.0000009) ? " <" : " ", 
			(1.0/(1<<(index - 8)) < 0.0000009) ? 1.0/(1<<(index - 8)) + 0.000001 : 1.0/(1<<(index - 8)));
	}
	return buffer;
}

char* double_tooltips(u8 count, u8 index, const char* values)
{
	if (count != 64)
		return 0;
	char* buffer = new char[64];
	if (!index)
		sprintf(
			buffer, 
			"%s: %s", 
			_(id::g_sign), 
			(values[index] == '0') ? _(id::g_plus) : _(id::g_minus));
	else if (index < 11)
	{
		u16 exponent = 0;
		u16 exponent_bit_index = 12;
		while (--exponent_bit_index)
			if (values[exponent_bit_index] == '1')
				exponent += 1<<(11 - exponent_bit_index);
		exponent <<= 5;
		exponent -= 1023 << 5;
		exponent >>= 5;
		if (exponent & 1 << 10)
			exponent += 0b1111100000000000;
		sprintf(
			buffer, 
			"%s: %i, %s: %i", 
			_(id::f_exponent), 
			(i16)exponent, 
			_(id::g_bit_value), 
			1<<(12 - index));
	}
	else
	{
		f64 normalized_mantissa = 1.0;
		u8 mantissa_index = 11;
		while (++mantissa_index < 64)
			if (values[mantissa_index] == '1')
				normalized_mantissa += 0.5/(1ull<<(mantissa_index - 12));
		sprintf(
			buffer, 
			"%s: %f, %s:%s%f", 
			_(id::f_mantissa), 
			normalized_mantissa, 
			_(id::g_bit_value), 
			(1.0/(1ull<<(index - 11)) < 0.0000009) ? " <" : " ", 
			(1.0/(1ull<<(index - 11)) < 0.0000009) ? 1.0/(1ull<<(index - 11)) + 0.000001 : 1.0/(1ull<<(index - 11)));
	}
	return buffer;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int argc, char** argv)
{
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

	// Set window to be fixed size
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(
		window_w * ((float)gui_size/20 + 1), 
		window_h * ((float)gui_size/20 + 1), 
		"int-conv", 
		nullptr, 
		nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Settings change flag
	bool is_setting_changed = true;

	// Force set selected tab
	bool force_select_tab = false;

	// Path to file with saved settings
	const char* settings_path = replaceFilename(*argv, "int-conv.settings");

	loadSettings(settings_path);

	// Set Terminus font (from terminusttf.h)
	ImFontConfig font_cfg;
	font_cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bitmap; // this fixes everything lmao
	ImFont* font_terminus = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
		FONT_TERMINUS_compressed_data_base85,
		font_size,
		&font_cfg,
		default_plus_polish_glyph_ranges);
	io.FontDefault = font_terminus;

	// Current tab variable
	i32 loaded_tab = selected_tab;

	// Local variables that stores inputted numbers
	u64 i_decimal = 0;
	i32 i_size = 8;
	char* i_sm_binary = convert::integerToSMBinary(i_decimal, i_size);
	char* i_oc_binary = convert::integerToOCBinary(i_decimal, i_size);
	char* i_tc_binary = convert::integerToTCBinary(i_decimal, i_size);
	f64 f_floating_point = 0;
	char* f_float_binary = convert::floatToBinary(f_floating_point);
	char* f_double_binary = convert::doubleToBinary((double)f_floating_point);

	// Local variables that stores last bit clicked or hovered in specified bit field
	i8 i_sm_lbi = -1;
	i8 i_oc_lbi = -1;
	i8 i_tc_lbi = -1;
	i8 f_f_lbi = -1;
	i8 f_d_lbi = -1;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Get ImGui default window background color
	u32 color_default = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);

	bool demo_open = false;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Set the window size, only if the demo window isn't open - we also enable resizing when that is the case
		if (!demo_open) glfwSetWindowSize(
			window, 
			window_w * ((float)gui_size/20 + 1), 
			window_h * ((float)gui_size/20 + 1));

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Set size of a ImGUI window to window size
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		{
			ImGui::Begin("Main Window", NULL, 
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
			
			ImGui::SetWindowFontScale((float)(gui_size-1)/20 + 1);
			if (ImGui::BeginTabBar("MainTabBar"))
			{
				if (ImGui::BeginTabItem(_(id::integers), NULL, (loaded_tab == 1) ? ImGuiTabItemFlags_SetSelected : 0))
				{
					selected_tab = 1;
					ImGui::TextUnformatted(_(id::i_decimal));
					if (ImGui::InputScalar("##i_decimal", ImGuiDataType_S64, &i_decimal))
					{
						delete[] i_sm_binary; i_sm_binary = convert::integerToSMBinary(i_decimal, i_size);
						delete[] i_oc_binary; i_oc_binary = convert::integerToOCBinary(i_decimal, i_size);
						delete[] i_tc_binary; i_tc_binary = convert::integerToTCBinary(i_decimal, i_size);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::g_size_in_bits));
					if (ImGui::InputInt("##i_size", &i_size))
					{
						if (i_size < 2)
							i_size = 2;
						else if (i_size > 64)
							i_size = 64;
						delete[] i_sm_binary; i_sm_binary = convert::integerToSMBinary(i_decimal, i_size);
						delete[] i_oc_binary; i_oc_binary = convert::integerToOCBinary(i_decimal, i_size);
						delete[] i_tc_binary; i_tc_binary = convert::integerToTCBinary(i_decimal, i_size);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::i_sign_magnitude));
					if ((i_sm_lbi = print_bits(
						i_sm_binary, 
						i_size, 
						color_bit_sections && invert_color, 
						1ull<<63, 
						((color_bit_sections && invert_color) || !(color_bit_sections && invert_color)) ? color_default : color_white, 
						(color_bit_sections) ? color_light_blue : color_white, 
						(color_bit_sections) ? color_dark_blue : color_white, 
						(multibit_invertion) ? i_sm_lbi : -1, 
						(show_bit_weights) ? integer_tooltips : 0)) != -1)
					{
						convert::SMBinaryToInteger(i_sm_binary, 64, &i_decimal);
						delete[] i_oc_binary; i_oc_binary = convert::integerToOCBinary(i_decimal, i_size);
						delete[] i_tc_binary; i_tc_binary = convert::integerToTCBinary(i_decimal, i_size);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::i_ones_completion));
					if ((i_oc_lbi = print_bits(
						i_oc_binary, 
						i_size, 
						color_bit_sections && invert_color, 
						1ull<<63, 
						((color_bit_sections && invert_color) || !(color_bit_sections && invert_color)) ? color_default : color_white, 
						(color_bit_sections) ? color_light_blue : color_white, 
						(color_bit_sections) ? color_dark_blue : color_white,  
						(multibit_invertion) ? i_oc_lbi : -1, 
						(show_bit_weights) ? integer_tooltips : 0)) != -1)
					{
						convert::OCBinaryToInteger(i_oc_binary, 64, &i_decimal);
						delete[] i_sm_binary; i_sm_binary = convert::integerToSMBinary(i_decimal, i_size);
						delete[] i_tc_binary; i_tc_binary = convert::integerToTCBinary(i_decimal, i_size);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::i_twos_completion));
					if ((i_tc_lbi = print_bits(
						i_tc_binary, 
						i_size, 
						color_bit_sections && invert_color, 
						1ull<<63, 
						((color_bit_sections && invert_color) || !(color_bit_sections && invert_color)) ? color_default : color_white, 
						(color_bit_sections) ? color_light_blue : color_white, 
						(color_bit_sections) ? color_dark_blue : color_white,  
						(multibit_invertion) ? i_tc_lbi : -1, 
						(show_bit_weights) ? integer_tooltips : 0)) != -1)
					{
						convert::TCBinaryToInteger(i_tc_binary, 64, &i_decimal);
						delete[] i_sm_binary; i_sm_binary = convert::integerToSMBinary(i_decimal, i_size);
						delete[] i_oc_binary; i_oc_binary = convert::integerToOCBinary(i_decimal, i_size);
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(_(id::floats), NULL, (loaded_tab == 2) ? ImGuiTabItemFlags_SetSelected : 0))
				{
					selected_tab = 2;
					ImGui::TextUnformatted(_(id::f_floating_point));
					ImGui::SetNextItemWidth(18*font_size*((float)gui_size/20 + 1));
					if (ImGui::InputScalar("##f_floating_point", ImGuiDataType_Double, &f_floating_point))
					{
						delete[] f_float_binary; f_float_binary = convert::floatToBinary((f32)f_floating_point);
						delete[] f_double_binary; f_double_binary = convert::doubleToBinary(f_floating_point);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::f_float));
					if ((f_f_lbi = print_bits(
						f_float_binary, 
						32, 
						color_bit_sections && invert_color, 
						0x807FFFFFFFFFFFFF, 
						((color_bit_sections && invert_color) || !(color_bit_sections && invert_color)) ? color_default : color_white, 
						(color_bit_sections) ? color_light_blue : color_white, 
						(color_bit_sections) ? color_dark_blue : color_white,  
						(multibit_invertion) ? f_f_lbi : -1, 
						(show_bit_weights) ? float_tooltips : 0)) != -1)
					{
						f32 f;
						convert::binaryToFloat(f_float_binary, &f);
						f_floating_point = (double)f;
						delete[] f_double_binary; f_double_binary = convert::doubleToBinary(f_floating_point);
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::f_double));
					if ((f_d_lbi = print_bits(
						f_double_binary, 
						64, 
						color_bit_sections && invert_color, 
						0x800FFFFFFFFFFFFF, 
						((color_bit_sections && invert_color) || !(color_bit_sections && invert_color)) ? color_default : color_white, 
						(color_bit_sections) ? color_light_blue : color_white, 
						(color_bit_sections) ? color_dark_blue : color_white,  
						(multibit_invertion) ? f_d_lbi : -1, 
						(show_bit_weights) ? double_tooltips : 0)) != -1)
					{
						convert::binaryToDouble(f_double_binary, &f_floating_point);
						delete[] f_float_binary; f_float_binary = convert::floatToBinary((f32)f_floating_point);
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(_(id::settings), NULL, (loaded_tab == 4) ? ImGuiTabItemFlags_SetSelected : 0))
				{
					selected_tab = 4;
					ImGui::TextUnformatted(_(id::s_language));
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_language_description));
					if (ImGui::BeginCombo("##language", _(id::lang_name)))
					{
						for (u16 i = 0; i < I18n::lang_count; i++)
						{
							I18n::lang_id cur_lang = (I18n::lang_id)i;
							if (ImGui::Selectable(I18n::get_str(cur_lang, id::lang_name), translator.lang == cur_lang))
							{
								translator.lang = cur_lang;
								loaded_tab = 4;
								force_select_tab = true;
							}
						}
						ImGui::EndCombo();
					}
					ImGui::TextUnformatted("\n");
					ImGui::TextUnformatted(_(id::s_gui_size));
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_gui_size_description));
					ImGui::SetNextItemWidth(font_size*6+3*((float)gui_size/20 + 1));
					if (ImGui::InputInt("##gui_size", &gui_size, 1))
					{
						if (gui_size < 1)
							gui_size = 1;
						else if (gui_size > 20)
							gui_size = 20;
					}
					ImGui::TextUnformatted("\n");
					ImGui::Checkbox(_(id::s_multibit_invertion), &multibit_invertion);
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_multibit_invertion_description));
					ImGui::Checkbox(_(id::s_show_bit_weights), &show_bit_weights);
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_show_bit_weights_description));
					ImGui::Checkbox(_(id::s_color_bit_sections), &color_bit_sections);
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_color_bit_sections_description));
					ImGui::BeginDisabled(!color_bit_sections);
					ImGui::Checkbox(_(id::s_invert_color), &invert_color);
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_invert_color_description));
					ImGui::EndDisabled();
					ImGui::Checkbox(_(id::s_show_option_help), &show_option_help);
					if (ImGui::IsItemHovered() && show_option_help)
						ImGui::SetTooltip("%s", _(id::s_show_option_help_description));

					ImGui::Checkbox("Debug: Show ImGui demo window", &demo_open); // TODO: i18n
					glfwSetWindowAttrib(window, GLFW_RESIZABLE, demo_open);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(_(id::about), NULL, (loaded_tab == 5) ? ImGuiTabItemFlags_SetSelected : 0))
				{
					loaded_tab = 0;
					selected_tab = 5;
					ImGui::TextUnformatted(_(id::a_text));
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			if (demo_open)
            	ImGui::ShowDemoWindow(&demo_open);

			ImGui::End();
		}

		if (!force_select_tab)
			loaded_tab = 0;
		else
			force_select_tab = false;

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

	// Save the settings
	saveSettings(settings_path);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}