#include <fstream>
#include <stdio.h>
#include "i18n/i18n.h"
#include "settings.h"

// class constructor
Settings::Settings(I18n::Translator* t)
{
    translator = t;
}

// Save current settings to file.
// If path is a null pointer, nothing happens.
void Settings::save(const char* path)
{
	FILE* f = fopen(path, "w");
	if (!f)
		return;
	fprintf(f, "%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i\n", 
		(u16)translator->lang,
		gui_size,
		multibit_inversion,
		show_bit_weights,
		color_bit_sections,
		invert_color,
		show_option_help,
		selected_tab);
	fclose(f);
}

// Load variables from file.
// If path is a null pointer or the file doesn't exist or has incorrect values, default settings are loaded.
void Settings::load(const char* path)
{
	u16 tmp_lang = 1;
	std::fstream f(path);
	if(f.is_open())
	{
		f
		>>tmp_lang
		>>gui_size
		>>multibit_inversion
		>>show_bit_weights
		>>color_bit_sections
		>>invert_color
		>>show_option_help
		>>selected_tab;
		f.close();
	}
	if (tmp_lang <= 2)
		translator->lang = (I18n::lang_id)tmp_lang;
	if (gui_size < 1 || gui_size > 20)
		gui_size = 1;
	if (selected_tab < 1 || selected_tab > 5)
		selected_tab = 1;
}
