#ifndef SERVER
#include "FontRegistry.h"
#include "GameEngine.h"

FontRegistry::FontRegistry()
{
	fonts = new std::vector< Font*>;
}


FontRegistry::~FontRegistry()
{
	for (std::vector<Font*>::iterator i = fonts->begin(); i != fonts->end(); i++)
		delete *i;
	fonts->clear();
	delete fonts;
}

void FontRegistry::registerFont(Font* font) {
	fonts->push_back(font);
	CGE::instance->logger->info("Registered font " + font->res.full);
}
Font* FontRegistry::getFont(_R& res) {
	static Font* s = NULL;
	if (s != NULL && s->res.full == res.full)
		return s;
	for (std::vector<Font*>::iterator i = fonts->begin(); i != fonts->end(); i++) {
		if ((*i)->res.full == res.full) {
			s = *i;
			return *i;
		}
	}
	Font* font = (*fonts)[0];
	CGE::instance->logger->warn("Accessed unknown font(" + res.full + "), returning first item(" + font->res.full + ")");
	return font;
}
#endif