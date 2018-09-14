#include "vec.h"

float distanceSquared(const vec2& A, const vec2& B)
{
	return (B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y);
}

vec3::vec3()
{

}

vec3::vec3(float x, float y, float z) : x(x), y(y), z(z)
{

}

vec3::vec3(vec2 V, float z) : x(V.x), y(V.y), z(z)
{

}

vec3 vec3::operator+(const vec3& V)
{
	return vec3(V.x + x, V.y + y, V.z + z);
}

vec3 vec3::operator-(const vec3& V)
{
	return vec3(x - V.x, y - V.y, z - V.z);
}

vec2 operator*(ivec2 V, float f)
{
	return vec2(V.x * f, V.y * f);
}

vec2 operator*(float f, ivec2 V)
{
	return vec2(V.x * f, V.y * f);
}