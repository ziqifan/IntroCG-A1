#pragma once

#include <MiniMath/Core.h>
#include <string>

class Transform
{
public:
	float m_pScale;

	float m_pRotX = 0.0f;
	float m_pRotY = 0.0f;
	float m_pRotZ = 0.0f; // local rotation angles

	vec3 m_pLocalPosition;

	// TODO: Compute these matrices in the Update function
	mat4 m_pLocalRotation;
	mat4 m_pLocalToWorldMatrix;


	Transform();
	~Transform();

	void setPosition(vec3 newPosition);
	void setRotationAngleX(float newAngle);
	void setRotationAngleY(float newAngle);
	void setRotationAngleZ(float newAngle);
	void setScale(float newScale);

	vec3 getPosition();
	float getRotationAngleY();

	mat4 getLocalToWorldMatrix();

	// TODO: Implement this update function
	virtual void update(float dt);	
	
	// Other Properties
	std::string name;
	vec4 color; 
};