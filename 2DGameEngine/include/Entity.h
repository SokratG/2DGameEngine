#pragma once
#include "DirectGraphic.h"
#include "Input.h"
#include "Image.h"
#include "Audio.h"


using Rect = struct Rect {
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
	explicit Rect() : left(0), top(0), right(0), bottom(0) {}
	Rect(int32 _left, int32 _top, int32 _right, int32 _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) {}
};

RECT Edge2WinRect(const Rect& edge);
Rect WinRect2Edge(const RECT& rt);

namespace GameEngine {
	class Game2D;

	enum COLLISION_TYPE {NONE, CIRCLE, BOX, ROTATED_BOX};
	const real32 GRAVITY = 6.67428e-11f; // N(m/kg)^2 // 9.81
	

	class Entity : public Image
	{
	private:
		using PhysicalObject = struct PhysicalObject {
			Rect edge;				
			Vec2<real32> center;	// center of entity
			Vec2<real32> velocity; // velocity of the object
			Vec2<real32> deltaV;	// added to velocity during next call to update()
			Vec2<real32> edge1, edge3;	// for compute rotated box
			real32 edge1Min, edge1Max, edge3Min, edge3Max; // min and max projections
			bool rotatedBoxReady;
			Vec2<real32> corners[4];
			real32 radius;			// radius of collision circle
			Vec2<real32> distSquared;
			real32 sumRadiiSquared;
			real32 rr; // radius square 
			real32 mass;
			real32 force; // force of gravity
			real32 gravity; 
			PhysicalObject() : edge1Min(0), edge1Max(0), edge3Min(0), edge3Max(0),
							rotatedBoxReady(false), radius(1), sumRadiiSquared(0), mass(1.), 
							force(0), gravity(GRAVITY), rr(0), edge(-1, -1, 1, 1) {}
		};
	protected:
		PhysicalObject physObj;
		COLLISION_TYPE collisionType;
		bool active;
		real32 scale;
		real32 health;
		Input* input;
		Audio* audio;
		HRESULT result;

		// Circular collision detection 
		virtual bool collideCircle(Entity& other, Vec2<real32>& collisionVector);
		// Axis aligned bounding box(AABB) collision detection 
		virtual bool collideBox(Entity& other, Vec2<real32>& collisionVector);
		// collision detection between boxes
		virtual bool collideRotatedBox(Entity& other, Vec2<real32>& collisionVector);
		// collision detection between box and circle
		virtual bool collideRotatedBoxCircle(Entity& other, Vec2<real32>& collisionVector);
		// collision detection helper functions
		void ComputeRotatedBox(real32 angle);
		bool projectionOverlap(Entity& other);
		bool collideCornerCircle(Vec2<real32> corner, Entity& other, Vec2<real32>& collisionVector);

	public:
		Entity(COLLISION_TYPE type = COLLISION_TYPE::NONE);
		virtual ~Entity(){}
		bool initialize(Game2D* pGame, uint32 width, uint32 height, uint32 ncol, TextureManager* pTextureM);
		virtual void activate() {active = true;}
		// return center Entity
		virtual Vec2<real32>* getCenter() noexcept {
			physObj.center = Vec2<real32>(Image::getCenterX(), Image::getCenterY());
			return &physObj.center;
		}
		// Return corner c of ROTATED_BOX
		virtual Vec2<real32>* getCorner(UINT c) noexcept {
			if (c >= 4) c = 0;
			return &physObj.corners[c];
		}
		// Return radius of circle.
		virtual real32 getRadius() const { return physObj.radius; }
		// Return RECT structure used for BOX and ROTATED_BOX 
		virtual RECT getEdge() const { return Edge2WinRect(physObj.edge); }
		// Return velocity vector.
		virtual Vec2<real32> getVelocity() const { return physObj.velocity; }
		// Return active.
		virtual bool getActive() const { return active; }
		// Return mass.
		virtual real32 getMass() const { return physObj.mass; }
		// Return gravitational constant.
		virtual real32 getGravity() const { return physObj.gravity; }
		// Return health;
		virtual real32 getHealth() const { return health; }
		// Return collision type (NONE, CIRCLE, BOX, ROTATED_BOX)
#pragma warning(push)
#pragma warning(disable : 26812)
		virtual COLLISION_TYPE getCollisionType() const { return collisionType; }
#pragma warning(pop)
		// Set velocity.
		virtual void setVelocity(Vec2<real32> v) { physObj.velocity = v; }
		// Set delta velocity. Added to velocity in update().
		virtual void setDeltaV(Vec2<real32> dv) { physObj.deltaV = dv; }
		// Set active.
		virtual void setActive(bool a) { active = a; }
		// Set health.
		virtual void setHealth(float h) { health = h; }
		// Set mass.
		virtual void setMass(real32 m) { physObj.mass = m; }
		// Set gravitational constant. Default is 6.67428e-11
		virtual void setGravity(real32 g) { physObj.gravity = g; }
		// Set radius of collision circle.
		virtual void setRadius(real32 r) { physObj.radius = r; }
		// Set collision type.
		virtual void setCollisionType(COLLISION_TYPE type) { collisionType = type; }
		// set rectangle of collision
		virtual void setRectEdge(RECT& rt) { physObj.edge = WinRect2Edge(rt); }

		virtual void update(real32 dt); // dt - delta time
		// AI - control
		virtual void ai(real32 dt, Entity& other) {}
		// collides check
		virtual bool collidesWith(Entity& other, Vec2<real32>& collisionVec);
		virtual bool outsideRect(RECT rec);
		virtual void damage(int delta) {}
		// bounce after collision to other entity
		void bounce(Entity& other, Vec2<real32>& collisionVec);
		// Force of gravity on this entity from other entity
		void gravityForce(Entity& other, real32 dt);

	};


}



