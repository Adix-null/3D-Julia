#pragma once
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

static vec4 var = vec4(0, 0, 0, 0);
static vec3 var2 = vec3(0, 0, 0);

//function inj

//Funkcija tiesiskai interpoliuojanti verte vienam intervale i kita
inline float map(float OrMin, float OrMax, float NewMin, float NewMax, float val)
{
	return ((val - OrMin) * (NewMax - NewMin) / (OrMax - OrMin)) + NewMin;
}

//Tasko sukimas 3d erdveje 
inline vec3 rotate(vec3 rot, vec3 pt)
{
	mat3 identity = mat3(
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0
	);
	mat3 rotx, roty, rotz;
	rotx = identity;
	roty = identity;
	rotz = identity;

	if (rot.x != 0)
	{
		float sx = sin(rot.x);
		float cx = cos(rot.x);
		rotx = mat3(
			1.0, 0.0, 0.0,
			0.0, cx, sx,
			0.0, -sx, cx
		);
	}
	if (rot.y != 0)
	{
		float sy = sin(rot.y);
		float cy = cos(rot.y);
		roty = mat3(
			cy, 0.0, -sy,
			0.0, 1.0, 0.0,
			sy, 0.0, cy
		);
	}
	if (rot.z != 0)
	{
		float sz = sin(rot.z);
		float cz = cos(rot.z);
		rotz = mat3(
			cz, sz, 0.0,
			-sz, cz, 0.0,
			0.0, 0.0, 1.0
		);
	}

	return pt * rotx * roty * rotz;
}

//Vertimai tarp RGB ir HSL spalvos erdviu
inline vec3 rgb2hsl(vec3 c) {
	float h = 0.0;
	float s = 0.0;
	float l = 0.0;
	float r = c.r;
	float g = c.g;
	float b = c.b;
	float cMin = min(r, min(g, b));
	float cMax = max(r, max(g, b));

	l = (cMax + cMin) / 2.0;
	if (cMax > cMin) {
		float cDelta = cMax - cMin;

		s = l < .0 ? cDelta / (cMax + cMin) : cDelta / (2.0 - (cMax + cMin));

		if (r == cMax) {
			h = (g - b) / cDelta;
		}
		else if (g == cMax) {
			h = 2.0 + (b - r) / cDelta;
		}
		else {
			h = 4.0 + (r - g) / cDelta;
		}

		if (h < 0.0) {
			h += 6.0;
		}
		h = h / 6.0;
	}
	return vec3(h, s, l);
}

inline vec3 hsl2rgb(vec3 c)
{
	vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.x + vec3(k.x, k.y, k.z)) * vec3(6.0) - vec3(k.w));
	return c.z * mix(vec3(k.x), clamp(p - vec3(k.x), vec3(0.0), vec3(1.0)), vec3(c.y));
}

//Operacijos tarp dvieju objektu
inline float unite(float d1, float d2)
{
	return min(d1, d2);
}
inline float subtract(float d1, float d2)
{
	return max(d1, -d2);
}
inline float intersect(float d1, float d2)
{
	return max(d1, d2);
}

inline vec3 infinite(vec3 pos, float c)
{
	return mod(pos + c, c * 2) - c;
}

//Ivairios SDF funkcijos
inline float distMandelbulb(vec3 pos) 
{
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	float power = 3.0 + var.x;

	for (int i = 0; i < 15; i++) 
	{
		r = length(z);

		if (r > 2.0) 
		{
			break;
		}

		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y, z.x);
		dr = pow(r, power - 1.0) * power * dr + 1.0;

		// scale and rotate the point
		float zr = pow(r, power);
		theta *= power;
		phi *= power;

		// convert back to cartesian coordinates
		z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
		z += pos;
	}
	return 0.5 * log(r) * r / dr;
}

inline float distPlane(vec3 pos, float h)
{
	return abs(pos.z - h);
}

inline float distSphere(vec3 pos, float rad)
{
	return length(pos) - rad;
}

inline float distBox(vec3 pos, vec3 box, vec3 pz)
{
	vec3 q = abs(pos + pz) - box;
	return length(max(q, vec3(0.0))) + min(max(q.x, max(q.y, q.z)), float(0.0));
}

inline float distBoxFrame(vec3 pos, vec3 b, float e)
{
	pos = abs(pos) - b;
	vec3 q = abs(pos + e) - e;
	return min(min(
		length(max(vec3(pos.x, q.y, q.z), vec3(0.0))) + min(max(pos.x, max(q.y, q.z)), 0.0f),
		length(max(vec3(q.x, pos.y, q.z), vec3(0.0))) + min(max(q.x, max(pos.y, q.z)), 0.0f)),
		length(max(vec3(q.x, q.y, pos.z), vec3(0.0))) + min(max(q.x, max(q.y, pos.z)), 0.0f));
}

inline float distMenger(vec3 pos)
{
	float d = distBox(pos, vec3(1.0), vec3(0));

	float s = 1.0;
	for (int m = 0; m < 8; m++)
	{
		vec3 a = mod(pos * vec3(s), vec3(2.0)) - vec3(1.0);
		s *= 3.0;
		vec3 r = abs(vec3(1.0) - vec3(3.0) * abs(a));

		float da = max(r.x, r.y);
		float db = max(r.y, r.z);
		float dc = max(r.z, r.x);
		float c = (min(da, min(db, dc)) - 1.0) / s;

		d = max(d, c);
	}
	return d;
}

inline vec3 boxFold(vec3 pos, vec3 r)
{
	return clamp(pos, -r, r) * vec3(2.0) - pos;
}

inline vec4 sphereFold(vec3 pos, float dz)
{
	vec3 z = pos;
	float ddz = dz;
	float fixedRadius2 = 1.6;
	float minRadius2 = 0.4;
	float r2 = dot(z, z);
	if (r2 < minRadius2) {
		float temp = (fixedRadius2 / minRadius2);
		z *= temp;
		ddz *= temp;
	}
	else if (r2 < fixedRadius2) {
		float temp = (fixedRadius2 / r2);
		z *= temp;
		ddz *= temp;
	}
	return vec4(z, ddz);
}

inline float distMandelbox(vec3 pos)
{
	vec3 z = pos;
	float Iterations = var.y * 10;
	float Scale = var.x;
	vec3 offset = z;
	float dr = 1.0;
	float trap = 1e10;
	for (float n = 0.; n < Iterations; n++) {
		z = boxFold(z, vec3(0.8));       
		z = boxFold(z, vec3(1.1));      
		vec4 ress = sphereFold(z, dr);   
		z = vec3(ress.x, ress.y, ress.z);
		dr = ress.w;
		z = Scale * z + offset; 
		dr = dr * abs(Scale) + 1.0;
		trap = min(trap, length(z));
	}
	float r = length(z);
	return r / abs(dr);
}

//Kaleidoskopine iteruota funkciju sistema
inline float distKIFS(vec3 pos, vec3 c, vec3 r1, vec3 r2, float scale) 
{
	scale = max(1.0f, scale);
	vec3 z = pos;
	float bailout = 100;
	int i = 0;
	float r = z.x * z.x + z.y * z.y + z.z * z.z;
	for (i = 0; i < 15 && r < bailout; i++) 
	{		
		z = rotate(r1, z);

		z = abs(z);
		//swaps
		if (z.x - z.y < 0) { float x1 = z.y; z.y = z.x; z.x = x1; }
		if (z.x - z.z < 0) { float x1 = z.z; z.z = z.x; z.x = x1; }
		if (z.y - z.z < 0) { float y1 = z.z; z.z = z.y; z.y = y1; }

		z.z -= 0.5 * c.z * (scale - 1) / scale;
		z.z = -abs(-z.z);
		z.z += 0.5 * c.z * (scale - 1) / scale;

		z = rotate(r2, z);

		//z = scale * (z - c) + c;
		z.x = scale * z.x - c.x * (scale - 1);
		z.y = scale * z.y - c.y * (scale - 1);
		z.z = scale * z.z;

		/*if (z.z > 0.5 * c.z * (scale - 1))
			z.z -= c.z * (scale - 1);*/

		r = z.x * z.x + z.y * z.y + z.z * z.z;
	}
	return (sqrt(r) - 2) * pow(scale, -i);
}

inline float distSierpinski(vec3 pos)
{
	vec3 z = pos;
	float scale = 2;
	float bailout = 10;
	float r = z.x * z.x + z.y * z.y + z.z * z.z;
	int i;
	for (i = 0; i < 20 && r < bailout; i++)
	{		
		if (z.x + z.y < 0) {float  x1 = -z.y; z.y = -z.x; z.x = x1; }
		if (z.x + z.z < 0) {float  x1 = -z.z; z.z = -z.x; z.x = x1; }
		if (z.y + z.z < 0) {float  y1 = -z.z; z.z = -z.y; z.y = y1; }

		z = scale * z - (scale - 1);

		r = z.x * z.x + z.y * z.y + z.z * z.z;
	}
	return (sqrt(r) - 2) * pow(scale, -i);
}

inline vec4 qsqr(vec4 a)
{
	return vec4(a.x * a.x - a.y * a.y - a.z * a.z - a.w * a.w,
		2.0 * a.x * a.y,
		2.0 * a.x * a.z,
		2.0 * a.x * a.w);
}

inline float distJuliaAlt(vec3 pos, vec4 c)
{
	vec4 z = vec4(pos.x, pos.y, pos.z, 0);
	float md2 = 1;
	float mz2 = dot(z, z);

	for (int i = 0; i < 100; i++)
	{
		md2 *= 4.0 * mz2;
		z = qsqr(z) + c; // z  -> z^2 + c

		mz2 = dot(z, z);

		if (mz2 > 4.0) break;
	}

	return 0.25 * sqrt(mz2 / md2) * log(mz2);
}

inline float distJulia(vec3 pos)
{
	vec4 z = vec4(pos.x, pos.y, pos.z, 0);
	float md2 = 1;
	float mz2 = dot(z, z);
	vec4 c = vec4(pos.z, var2.y, 0, 0);

	for (int i = 0; i < 15; i++)
	{
		md2 *= 4.0 * mz2;
		z = qsqr(z) + c; // z  -> z^2 + c

		mz2 = dot(z, z);

		if (mz2 > 4.0) break;
	}

	return 0.25 * sqrt(mz2 / md2) * log(mz2);
}

//Pagrindinei app.cpp progrmamai perduodama funkcija
inline float distance(vec3 pos)
{
	vec3 inf = infinite(pos, 3);
	
	//return min(distBox(pos, vec3(1), vec3(0)), distPlane(pos));
	//return min(distBoxFrame(pos, vec3(1), 0.075), distPlane(pos));

	//return distBox(inf, vec3(0.5), vec3(0));
	//return distBox(pos, vec3(0.5), vec3(0));
	//return distSphere(pos, 3);
	/*vec3 o = vec3(0.5);
	if(var.x < 0)
		return unite(distSphere(pos, 0.5), distBox(pos, o, o));
	else if(var.x < 2)
		return intersect(distSphere(pos, 0.5), distBox(pos, o, o));
	else
		return subtract(distBox(pos, o, o), distSphere(pos, 0.5));*/
	 
	//return distMenger(pos);
	//return distSierpinski(pos);
	
	//return distMandelbox(pos);
	//return unite(distMandelbulb(pos), distPlane(pos, -2));
	//return distKIFS(pos, var2, vec3(var.x, var.y, var.z), vec3(0), var.w);
	///return distKIFS(pos, vec3(2, 4.8, 0), vec3(0, 0.43, 0), vec3(0, 0, 0), 1.3);
	 
	return subtract(distJuliaAlt(pos, var), distBox(pos, vec3(10, 10, 3), vec3(0, 0, -(var2.x + 3))));
	//return subtract(distJulia(pos), distBox(pos, vec3(10, 10, 3), vec3(0, 0, -(var2.x + 3))));
}