#include "Entity.h"
#include "Game2DEngine.h"

RECT Edge2WinRect(const Rect& edge)
{
	RECT rt;
	rt.top = edge.top; rt.bottom = edge.bottom;
	rt.right = edge.right; rt.left = edge.left;
	return rt;
}

Rect WinRect2Edge(const RECT& rt)
{
	Rect edge;
	edge.top = rt.top; edge.bottom = rt.bottom;
	edge.right = rt.right; edge.left = rt.left;
	return edge;
}

namespace GameEngine {

	Entity::Entity(COLLISION_TYPE type) : Image(), active(true), collisionType(type), health(100),
		input(nullptr), audio(nullptr)
	{
		result = E_FAIL;
		scale = 1;
	}

	bool Entity::initialize(Game2D* pGame, uint32 width, uint32 height, uint32 ncol, TextureManager* pTextureM)
	{
		input = pGame->getInput();
		audio = pGame->getAudio();
		return Image::initialize(pGame->getGraphicDriver(), width, height, ncol, pTextureM);
	}

	void Entity::update(real32 dt) {
		physObj.velocity += physObj.deltaV;
		physObj.deltaV.x = 0;
		physObj.deltaV.y = 0;
		Image::update(dt);
		physObj.rotatedBoxReady = false;
	}

	bool Entity::outsideRect(RECT rt)
	{
		real32 _scale = getScale();
		if (spriteData.x > rt.right || spriteData.y > rt.bottom ||
			spriteData.x + spriteData.width * _scale < rt.left ||
			spriteData.y + spriteData.height * _scale < rt.top)
			return true;
		return false;
	}

	bool Entity::collidesWith(Entity& other, Vec2<real32>& collisionVector)
	{
		if (!active || !other.getActive())
			return false;

		// type of object and choose specify method collision detection
		if (collisionType == CIRCLE && other.getCollisionType() == CIRCLE)
			return collideCircle(other, collisionVector);
		if (collisionType == BOX && other.getCollisionType() == BOX)
			return collideBox(other, collisionVector);
		if (collisionType == ROTATED_BOX && other.getCollisionType() == ROTATED_BOX)
			return collideRotatedBox(other, collisionVector);
		else {
			if (collisionType == CIRCLE) {
				bool collide = other.collideRotatedBoxCircle(*this, collisionVector);
				collisionVector.neg();
				return collide;
			}
			else
				return collideRotatedBoxCircle(other, collisionVector);
		}
		return false;
	}
	//=============================================================================
	// Circular collision detection 
	// Post: returns true if collision, false otherwise  sets collisionVector if collision
	//=============================================================================
	bool Entity::collideCircle(Entity& other, Vec2<real32>& collisionVector)
	{
		// calc difference between centers
		physObj.distSquared = *getCenter() - *other.getCenter();
		physObj.distSquared.x = physObj.distSquared.x * physObj.distSquared.x;
		physObj.distSquared.y = physObj.distSquared.y * physObj.distSquared.y;

		// calc the sum of the radii (adjusted for scale)
		physObj.sumRadiiSquared = (physObj.radius * getScale()) + (other.getRadius() * other.getScale());
		physObj.sumRadiiSquared *= physObj.sumRadiiSquared;

		if (physObj.distSquared.x + physObj.distSquared.y <= physObj.sumRadiiSquared)
		{
			collisionVector = *other.getCenter() - *getCenter();
			return true;
		}
		return false;
	}

	//=============================================================================
	// Axis aligned bounding box(AABB) collision detection 
	// Post: returns true if collision, false otherwise sets collisionVector if collision      
	//=============================================================================
	bool Entity::collideBox(Entity& other, Vec2<real32>& collisionVector)
	{
		// cache scale factor
		real32 scale1 = other.getScale();
		real32 scale2 = getScale();
		real32 X1 = physObj.center.x;
		real32 Y1 = physObj.center.y;
		real32 X2 = other.getCenterX();
		real32 Y2 = other.getCenterY();

		if ((X1 + physObj.edge.right * scale1 >= X2 + other.physObj.edge.left * scale2) &&
			(X1 + physObj.edge.left * scale1 <= X2 + other.physObj.edge.right * scale2) &&
			(Y1 + physObj.edge.bottom * scale1 >= Y2 + other.physObj.edge.top * scale2) &&
			(Y1 + physObj.edge.top * scale1 <= Y2 + other.physObj.edge.bottom * scale2))
		{
			collisionVector = *other.getCenter() - *getCenter();
			return true;
		}
		return false;
	}
	//=============================================================================
	// Compute corners of rotated box, projection edges and min and max projections
	// 0---1  corner numbers
	// |   |
	// 3---2
	//=============================================================================
	void Entity::ComputeRotatedBox(real32 angle)
	{
		if (physObj.rotatedBoxReady)
			return;
		real32 projection;
		real32 scale = getScale();

		Vec2<real32> RotX; Vec2<real32> RotY;
		RotX = Vec2<real32>::rotateX(angle);
		RotY = Vec2<real32>::rotateY(angle);

		Vec2<real32>* center = getCenter();

		physObj.corners[0] = *center + RotX * (physObj.edge.left * scale) + RotY * physObj.edge.top * scale;
		physObj.corners[1] = *center + RotX * (physObj.edge.right * scale) + RotY * physObj.edge.top * scale;
		physObj.corners[2] = *center + RotX * (physObj.edge.right * scale) + RotY * physObj.edge.bottom * scale;
		physObj.corners[3] = *center + RotX * (physObj.edge.left * scale) + RotY * physObj.edge.bottom * scale;

		// corners[0] is used as origin
		physObj.edge1 = Vec2<real32>(physObj.corners[1].x - physObj.corners[0].x, physObj.corners[1].y - physObj.corners[0].y);
		physObj.edge1 = physObj.edge1.normalize();
		physObj.edge3 = Vec2<real32>(physObj.corners[3].x - physObj.corners[0].x, physObj.corners[3].y - physObj.corners[0].y);
		physObj.edge3 = physObj.edge3.normalize();

		// this entities min and max projection onto edges
		projection = physObj.edge1.dot(physObj.corners[0]);
		physObj.edge1Min = projection;
		physObj.edge1Max = projection;
		// project onto edge1
		projection = physObj.edge1.dot(physObj.corners[1]);
		if (projection < physObj.edge1Min)
			physObj.edge1Min = projection;
		else if (projection > physObj.edge1Max)
			physObj.edge1Max = projection;
		// project onto edge3
		projection = physObj.edge3.dot(physObj.corners[0]);
		physObj.edge3Min = projection;
		physObj.edge3Max = projection;
		projection = physObj.edge3.dot(physObj.corners[3]);
		if (projection < physObj.edge3Min)
			physObj.edge3Min = projection;
		else if (projection > physObj.edge3Max)
			physObj.edge3Max = projection;

		physObj.rotatedBoxReady = true;
	}

	//=============================================================================
	// Projects other box onto this edge1 and edge3.
	//=============================================================================
	bool Entity::projectionOverlap(Entity& other)
	{
		real32 projection1, projection3, min1, max1, min3, max3;

		projection1 = physObj.edge1.dot(other.physObj.corners[0]); // project to corner 0
		min1 = projection1;
		max1 = projection1;
		projection3 = physObj.edge3.dot(other.physObj.corners[0]);
		min3 = projection3;
		max3 = projection3;

		for (size_t i = 0; i < 4; ++i) {
			// project corner onto edge1
			projection1 = physObj.edge1.dot(other.physObj.corners[i]);
			if (projection1 < min1)
				min1 = projection1;
			else if (projection1 > max1)
				max1 = projection1;
			// project corner onto edge3
			projection3 = physObj.edge3.dot(other.physObj.corners[i]);
			if (projection3 < min3)
				min3 = projection3;
			else if (projection3 > max3)
				max3 = projection3;
		}
		if (min1 > physObj.edge1Max || max1 < physObj.edge1Min) // if projections do not overlap
			return false;							  // no collision is possible
		if (min3 > physObj.edge3Max || max3 < physObj.edge3Min)
			return false;

		return true;
	}

	//=============================================================================
	// Rotated Box collision detection 
	//=============================================================================
	bool Entity::collideRotatedBox(Entity& other, Vec2<real32>& collisionVector)
	{
		ComputeRotatedBox(spriteData.angle);
		other.ComputeRotatedBox(other.spriteData.angle);

		if (other.projectionOverlap(*this) && projectionOverlap(other))
		{
			collisionVector - *other.getCenter() - *other.getCenter();
			return true;
		}
		return false;
	}
	//=============================================================================
	// The box corner is checked for collision with circle using a distance check.
	//=============================================================================
	bool Entity::collideCornerCircle(Vec2<real32> corner, Entity& other, Vec2<real32>& collisionVector)
	{
		Vec2<real32> dS = corner - *other.getCenter();
		dS.x = dS.x * dS.x;
		dS.y = dS.y * dS.y;
		physObj.distSquared = dS;

		// Calculate the sum of the radii, then square it
		real32 sumRadii = other.getRadius() * other.getScale();
		sumRadii = sumRadii * sumRadii;
		physObj.sumRadiiSquared = sumRadii;
		// check collide corner
		if (dS.x + dS.y <= sumRadii)
		{
			collisionVector = *other.getCenter() - corner;
			return true;
		}
		return false;
	}

	//=============================================================================
	// Rotated Box and Circle collision detection 
	// If the circle center is outside the lines extended from the collision box
	// edges (Voronoi region) then the nearest box corner is checked for collision using a distance check.
	//   Voronoi0 |   | Voronoi1
	//         ---0---1---
	//            |   |
	//         ---3---2---
	//   Voronoi3 |   | Voronoi2
	//=============================================================================
	bool Entity::collideRotatedBoxCircle(Entity& other, Vec2<real32>& collisionVector)
	{
		real32 min, max, center1, center3;
		ComputeRotatedBox(spriteData.angle);

		real32 RadiusScaleFactor = other.getRadius() * other.getScale();

		// project circle center on to edge1
		center1 = physObj.edge1.dot(*other.getCenter());
		min = center1 - RadiusScaleFactor;
		max = center1 + RadiusScaleFactor;
		if (min < physObj.edge1Max || max < physObj.edge1Min) // check project overlap
			return false;
		// edge3
		center3 = physObj.edge3.dot(*other.getCenter());
		min = center3 - RadiusScaleFactor;
		max = center3 + RadiusScaleFactor;
		if (min < physObj.edge3Max || max < physObj.edge3Min) // check project overlap
			return false;

		// check corners if collision box in Voronoi region
		if (center1 < physObj.edge1Min && center3 < physObj.edge3Min)
			return collideCornerCircle(physObj.corners[0], other, collisionVector);
		if (center1 > physObj.edge1Max && center3 < physObj.edge3Min)
			return collideCornerCircle(physObj.corners[1], other, collisionVector);
		if (center1 > physObj.edge1Max && center3 > physObj.edge3Max)
			return collideCornerCircle(physObj.corners[2], other, collisionVector);
		if (center1 < physObj.edge1Min && center3 > physObj.edge3Max)
			return collideCornerCircle(physObj.corners[3], other, collisionVector);

		// set collision vector, how vector between center circle and center box
		collisionVector = *other.getCenter() - *getCenter();
		return true;
	}


	void Entity::bounce(Entity& other, Vec2<real32>& collisionVec) 
	{
		real32 mass = other.getMass();
		Vec2<real32> Vdiff = other.getVelocity() - physObj.velocity; // difference vector between velocity
		Vec2<real32> cUV = collisionVec.normalize(); // take unit vector
		real32 dotUVVd = cUV.dot(Vdiff);
		
		real32 massRatio = 2.;
		if (physObj.mass != 0)
			massRatio *= (mass / (physObj.mass + mass));

		if (dotUVVd > 0) {// move entities apart along collision vector
			setX(getX() - cUV.x * massRatio);
			setY(getY() - cUV.y * massRatio);
		}
		physObj.deltaV += cUV * (massRatio * dotUVVd);
	}

	// Adds the gravitational force to the velocity vector of entity
	// force = GRAVITY * m1 * m2 / r*r
	//                    2              2
	//  r*r  =   (Ax - Bx)   +  (Ay - By)
	void Entity::gravityForce(Entity& other, real32 dt)
	{
		if (!active || !other.getActive())
			return;

		real32 CX = getCenterX();
		real32 CY = getCenterY();
		real32 OCX = other.getCenterX();
		real32 OCY = other.getCenterY();

		physObj.rr = ((OCX-CX)* (OCX - CX)) + ((OCY - CY) * (OCY - CY));
		physObj.force = physObj.gravity * other.getMass() * (getMass() / physObj.rr);
		
		// create gravity vector
		Vec2<real32> gV(OCX - CX, OCY - CY);
		gV = gV.normalize();
		gV = gV * (physObj.force * dt);
		physObj.velocity += gV;
	}
}