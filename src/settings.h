// settings interface

#ifndef _CPP_SETTINGS_H_
#define _CPP_SETTINGS_H_

#include "i18n/i18n.h"
#include "stdvar.h"

// class holding settings values
class Settings
{
	// pointer to an instance of I18n::Translator (to change language)
	I18n::Translator* translator;

	public:
		// Size of the entire GUI
		i32 gui_size = 1;
		// Invert multiple bits with one click and mouse movement
		bool multibit_inversion = false;
		// Show the tooltips
		bool show_bit_weights = true;
		// Color background of binary representation
		bool color_bit_sections = true;
		// Color background of bit fields
		bool invert_color = false;
		// Show tooltip after hovering on a setting
		bool show_option_help = true;
		// Current tab
		i32 selected_tab = 0;

		void save(const char* path);
		void load(const char* path);

		Settings(I18n::Translator* t);
};

#endif
