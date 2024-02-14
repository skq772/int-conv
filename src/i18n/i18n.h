// internationalization (language) functions
// i really really have no idea if this is the right thing to do
// and i dont LIIIKE how it's all in this file but ugh whatever

#ifndef _CPP_I18N_H_
#define _CPP_I18N_H_

#include "../stdvar.h"

namespace I18n
{
	enum class id
	{
		lang_name,
		g_sign,
		g_plus,
		g_minus,
		g_size_in_bits,
		g_bit_value,
		integers,
		i_decimal,
		i_sign_magnitude,
		i_ones_completion,
		i_twos_completion,
		floats,
		f_floating_point,
		f_exponent,
		f_mantissa,
		f_float,
		f_double,
		settings,
		s_language,
		s_language_description,
		s_gui_size,
		s_gui_size_description,
		s_multibit_invertion,
		s_multibit_invertion_description,
		s_show_bit_weights,
		s_show_bit_weights_description,
		s_color_bit_sections,
		s_color_bit_sections_description,
		s_invert_color,
		s_invert_color_description,
		s_show_option_help,
		s_show_option_help_description,
		about,
		a_text
	};

	enum class lang_id
	{
		en_us,
		pl_pl
	};

	const u16 lang_count = 2;
    const u16 text_count = 34;

	static const char *strings[lang_count][text_count] = {
		{
			"English (en_us)",
			"Sign",
			"plus",
			"minus",
			"Size in bits",
			"Bit value",
			"Integers",
			"Decimal number",
			"Sign-magnitude",
			"One's completion",
			"Two's completion",
			"Floats",
			"Floating-point number",
			"Exponent",
			"Mantissa",
			"Float",
			"Double",
			"Settings",
			"Language",
			"Set the interface language",
			"GUI scale",
			"Set the entire GUI scale",
			"Multibit invertion",
			"Invert many bits at once",
			"Show bit weights",
			"Show as tooltips weigths of bits",
			"Color bit sections",
			"Show bit field sections using color",
			"Invert color",
			"Color background of bit fields",
			"Show option help",
			"Show tooltip after hovering on a setting",
			"About",
			R"(Hello there!
You can find simple answers of simple questions here.

Q: What's even this?!
A: This is a simple number converter.

Q: What types of convertion can I do here?
A: It does custom, decimal/floating-point to binary
   and vice versa convertions.

Q: How to provide input?
A: You can write a number in number inputs
   and click on bits to invert those.

Q: What multibit invertion does?
A: This option allows user to invert
   multiple bits at once with just a mouse button holded
   and cursor hovering over them.)"
		},

	{
		"polski (pl_pl)",
		"Znak",
		"plus",
		"minus",
		"Rozmiar w bitach",
		"Wartość bita",
		"Integery",
		"Liczba dziesiętna",
		"Znak-moduł",
		"U1",
		"U2",
		"Zmiennoprzecinkowe",
		"Liczba zmiennoprzecinkowa",
		"Wykładnik",
		"Mantysa",
		"Float",
		"Double",
		"Ustawienia",
		"Język",
		"Ustaw język interfejsu",
		"Skala GUI",
		"Ustaw skalę całego okna",
		"Inwersja wielobitowa",
		"Inwertuj wiele bitów naraz",
		"Pokazuj wagi bitów",
		"Wyświetlaj w dymkach wagi poszczególnych bitów",
		"Koloruj sekcje bitów",
		"Pokaż sekcje pól bitowych za pomocą koloru",
		"Odwróć kolor",
		"Koloruj tło bitów",
		"Pokaż podpowiedź do opcji",
		"Pokaż dymek z opisem po najechaniu na opcję",
		"O programie",
		R"(
Witaj!
Znajdziesz tu proste odpowiedzi na proste pytania.

P: Co to niby ma być?!
O: To jest prosty konwerter do liczb.

P: Co to umie zrobić?
O: Potrafi przekonwertować liczby dowolne,
   dziesiętne/zmiennoprzecinkowe na binarny i na odwrót.

P: Jak wprowadzać liczby?
O: Wpisujesz liczbę dziesiętną w pole,
   a kliknięcie w odpowiedni bit odwraca jego wartość.

P: Co robi opcja \"Inwersja wielobitowa\"?
O: Opcja ta umożliwia użytkownikowi na odwróceniu
   wartości wielu bitów na raz poprzez tylko przytrzymaniu
   przycisku myszy i najechaniu na nie.)"
		}
	};

	// get a string from a language
	const char* get_str(lang_id lang, id str);

	// the purpose of this class is mainly to hold the current language
	class Translator
	{
		public:
			// current language index
			// TODO: setter/getter?
			lang_id lang = lang_id::en_us;

			// get a string from the current language
			inline const char* get_str(id str);
	};
}

#endif
