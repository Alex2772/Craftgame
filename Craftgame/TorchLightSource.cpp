#include "TorchLightSource.h"
#include "GameEngine.h"


void TorchLightSource::tick()
{
	static glm::vec3 color1 = glm::vec3(Utils::fromHex(0xff941bff));
	static glm::vec3 color2 = glm::vec3(Utils::fromHex(0x86360dff));

	m += CGE::instance->millis / 1.2f * ((float(rand() % 11) / 5.f) - 1);
	m = glm::clamp(m, 0.f, 1.f);

	color = glm::mix(color1, color2, m) * 8.f;
}