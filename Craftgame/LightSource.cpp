
#include "LightSource.h"
#include "GameEngine.h"

LightSource::LightSource(glm::vec3 _p, glm::vec3 _c, float d) :
	pos(_p),
	color(_c),
	distance(d){
#ifndef SERVER
	ts<std::vector<LightSource*>>::rw v(CGE::instance->light);
	v->push_back(this);
#endif
}

LightSource::~LightSource()
{
#ifndef SERVER
	ts<std::vector<LightSource*>>::rw v(CGE::instance->light);
	std::vector<LightSource*>::iterator k = std::find(v->begin(), v->end(), this);
	if (k != v->end())
		v->erase(k);
#endif
}