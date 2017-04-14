#ifndef SERVER
#include "AdvancedModel.h"

ModelObject::ModelObject() {}

AdvancedModel::AdvancedModel(_R _r) :
	resource(_r) {

}
AdvancedModel::~AdvancedModel() {
	for (int i = 0; i < objects.size(); i++) {
		delete objects[i];
	}
	objects.clear();
}
#endif