#pragma once

#include <cmath>

#undef M_PI
#define M_PI       3.14159265358979323846f

struct irect
{
	int x, y, w, h;
};

struct ivec2
{
	ivec2(){}
	ivec2(int X, int Y) : x(X), y(Y) {}

	int length() { return (int)sqrt(x * x + y * y); }
	int max() { int xAbs = abs(x); int yAbs = abs(y); return xAbs > yAbs ? xAbs : yAbs; }

	int x, y;

};

inline bool operator==(const ivec2& A, const ivec2& B)
{
	return A.x == B.x && A.y == B.y;
}

inline bool operator!=(const ivec2& A, const ivec2& B)
{
	return A.x != B.x || A.y != B.y;
}

inline ivec2 operator+(const ivec2& A, const ivec2& B)
{
	return ivec2(A.x + B.x, A.y + B.y);
}

inline ivec2 operator-(const ivec2& A, const ivec2& B)
{
	return ivec2(A.x - B.x, A.y - B.y);
}

inline ivec2 operator*(const ivec2& A, int f)
{
	return ivec2(A.x * f, A.y * f);
}

struct vec2
{
	vec2() { }

	constexpr vec2(float inX, float inY) : x(inX), y(inY) { }

	inline float sizeSquared() const { return x * x + y * y; }

	inline float size() const { return sqrt(sizeSquared()); }

	float angle() const { return atan2(y, x); }

	vec2 normalized() const
	{
		float norm = size();
		if (norm > 0)
			return vec2(x / norm, y / norm);
		return vec2(0, 0);
	}

	void operator+=(const vec2& V)
	{
		x += V.x;
		y += V.y;
	}

	void operator-=(const vec2& V)
	{
		x -= V.x;
		y -= V.y;
	}

	void operator*=(float f)
	{
		x *= f;
		y *= f;
	}

	bool operator!=(const vec2& other) const
	{
		return x != other.x || y != other.y;
	}

	float x, y;
};

inline vec2 operator/(const ivec2& A, float f)
{
	return vec2(A.x / f, A.y / f);
}


inline vec2 operator+(const vec2& A, const vec2& B)
{
	return vec2(A.x + B.x, A.y + B.y);
}

inline vec2 operator-(const vec2& A, const vec2& B)
{
	return vec2(A.x - B.x, A.y - B.y);
}

inline float operator*(const vec2& A, const vec2& B)
{
	return A.x * B.x + A.y * B.y;
}

inline vec2 operator*(const vec2& A, float f)
{
	return vec2(A.x * f, A.y * f);
}

inline vec2 operator/(const vec2& A, float f)
{
	return vec2(A.x / f, A.y / f);
}

inline vec2 operator*(float f, const vec2& A)
{
	return vec2(A.x * f, A.y * f);
}

float distanceSquared(const vec2& A, const vec2& B);

struct vec3
{
	float x, y, z;

	vec3();
	vec3(float x, float y, float z);
	vec3(vec2 V, float z);

	vec3 operator+(const vec3& V);
	vec3 operator-(const vec3& V);
};

vec2 operator*(ivec2 V, float f);
vec2 operator*(float f, ivec2 V);

inline vec2 toFloatVec(ivec2 V)
{
	return vec2((float)V.x, (float)V.y);
}

inline ivec2 toIntVec(vec2 V)
{
	return ivec2((int)V.x, (int)V.y);
}

inline ivec2 roundVecToInt(vec2 V)
{
	return ivec2((int)(V.x + 0.5f), (int)(V.y + 0.5f));
}