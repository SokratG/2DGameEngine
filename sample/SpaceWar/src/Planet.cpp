#include "Planet.h"


Planet::Planet() : Entity()
{
	spriteData.x = PlanetParam::X;
	spriteData.y = PlanetParam::Y;
	physObj.radius = PlanetParam::COLLISION_RADIUS;
	physObj.mass = PlanetParam::MASS;
	startFrame = PlanetParam::START_FRAME;
	endFrame = PlanetParam::END_FRAME;
	setCurrentFrame(startFrame);
}