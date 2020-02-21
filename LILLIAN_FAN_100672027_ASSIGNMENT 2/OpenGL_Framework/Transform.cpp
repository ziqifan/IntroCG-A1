#include "Transform.h"

Transform::Transform()
	: m_pScale(1.0f),
	color(vec4(1.0f))
{
}

Transform::~Transform() {}

void Transform::setPosition(vec3 newPosition)
{
	m_pLocalPosition = newPosition;
}

void Transform::setRotationAngleX(float newAngle)
{
	m_pRotX = newAngle;
}

void Transform::setRotationAngleY(float newAngle)
{
	m_pRotY = newAngle;
}

void Transform::setRotationAngleZ(float newAngle)
{
	m_pRotZ = newAngle;
}

void Transform::setScale(float newScale)
{
	m_pScale = newScale;
}

vec3 Transform::getPosition()
{
	return m_pLocalPosition;
}

float Transform::getRotationAngleY()
{
	return m_pRotY;
}

mat4 Transform::getLocalToWorldMatrix()
{
	return m_pLocalToWorldMatrix;
}

void Transform::update(float dt)
{
	// Create 4x4 transformation matrix

	// Create rotation matrix

	mat4 rx; 
	mat4 ry; 
	mat4 rz; 

	rx.RotateX(m_pRotX);
	ry.RotateY(m_pRotY);
	rz.RotateZ(m_pRotZ);
	// Note: pay attention to rotation order, ZYX is not the same as XYZ
	m_pLocalRotation = rz * ry * rx;

	// Create translation matrix
	mat4 tran; 
	tran.Translate(m_pLocalPosition);

	// Create scale matrix
	mat4 scale; 
	scale.Scale(m_pScale);

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a parent, then we must apply the parent's transform
	m_pLocalToWorldMatrix = tran * m_pLocalRotation * scale;
}