// internationalization (language) functions
#include "i18n.h"
#include "../stdvar.h"

namespace I18n
{
	const char* get_str(lang_id lang, id str)
	{
		return strings[(u16)lang][(u16)str];
	}
	
	inline const char* Translator::get_str(id str)
	{
		return I18n::get_str(lang, str);
	}
}
