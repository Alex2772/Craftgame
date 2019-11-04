#ifndef SERVER
#include "FontRegistry.h"
#include "GameEngine.h"

CFontRegistry::CFontRegistry()
{
	fonts = new std::vector< CFont*>;
}


CFontRegistry::~CFontRegistry()
{
	for (std::vector<CFont*>::iterator i = fonts->begin(); i != fonts->end(); i++)
		delete *i;
	fonts->clear();
	delete fonts;
}

void CFontRegistry::registerCFont(CFont* font) {
	fonts->push_back(font);
	CGE::instance->logger->info("Registered font " + font->res.full);
}
CFont* CFontRegistry::getCFont(_R res) {
	static CFont* s = NULL;
	if (s != NULL && s->res.full == res.full)
		return s;
	for (std::vector<CFont*>::iterator i = fonts->begin(); i != fonts->end(); i++) {
		if ((*i)->res.full == res.full) {
			s = *i;
			return *i;
		}
	}
	CFont* font = (*fonts)[0];
	CGE::instance->logger->warn("Accessed unknown font(" + res.full + "), returning first item(" + font->res.full + ")");
	return font;
}
#endif
