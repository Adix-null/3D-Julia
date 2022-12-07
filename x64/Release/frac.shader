#shader fragment
#version 330 core

uniform vec4 var;
//injection

float PI = 3.14159265358979;


in vec2 pos;

out vec4 color;

uniform vec3 style;

uniform vec3 camPos;
uniform vec3 camRot;

uniform float fov;
uniform vec2 aspect;

uniform float lightIntensity;
uniform int stepLimit;
uniform bool glow;
uniform bool coloring;
uniform int msaa;
uniform float glowMult;
uniform float AO;
uniform float eps;


float lengthLimit = 0.001;
float threshold = lengthLimit * glowMult;

float maxDist = 150;

vec3 lights[2];

float angle, intensity;
int stL;

vec4 march(vec3 rayPos, vec3 rayDir)
{
	int st = 0;
	float smallestDist = 10;
	vec3 clp = rayPos;
	float dist = 10;
	vec3 oldPos = rayPos;
	vec3 newPos;
	
	while (dist < maxDist && dist > lengthLimit && st < stepLimit)
	{
		st++;
		dist = distance(oldPos);
		newPos = oldPos + rayDir * dist;
		oldPos = newPos;
		if (dist < smallestDist)
		{
			smallestDist = dist;
			clp = oldPos;
		}
	}
	if (dist < lengthLimit)
		return vec4(oldPos, st);
	else
		return vec4(clp, 0);
}

float shadow(in vec3 ro, in vec3 rd, float mint, float maxt)
{
	float t = mint;
	while (t < maxt)
	{
		float h = distance(ro + rd * t);
		if (h < 0.001)
			return 0.0;
		t += h;
	}
	return 1.0;
}

float softShadow(in vec3 ro, in vec3 rd, float mint, float maxt, float k)
{
	float res = 1.0;
	float ph = 1e20;
	int st = 0;
	float t = mint;
	while (t < maxt && st < stepLimit)
	{
		st++;
		float h = distance(ro + rd * t);
		if (h < 0.001)
			return 0.0;
		float y = h * h / (2.0 * ph);
		float d = sqrt(h * h - y * y);
		res = min(res, k * d / max(0.0, t - y));
		ph = h;
		t += h;
	}
	return res;
}


vec3 compute(vec2 aaCoord)
{
	float d = pow(10, -eps);

	vec3 rd = rotate(camRot, normalize(vec3(
		sin((pos.x + aaCoord.x) * 0.5 * fov), 1,
		sin((pos.y + aaCoord.y) * 0.5 * fov * aspect.y / aspect.x))));
	vec4 dp = march(camPos, normalize(rd));

	if (dp.w != 0)
	{
		/*vec3 normal = normalize(distance(dp.xyz) - vec3(
			distance(dp.xyz - vec3(d, 0, 0)),
			distance(dp.xyz - vec3(0, d, 0)),
			distance(dp.xyz - vec3(0, 0, d))
		));*/

		vec3 normal = normalize(vec3(
			distance(vec3(dp.x + d, dp.y, dp.z)) - distance(vec3(dp.x - d, dp.y, dp.z)),
			distance(vec3(dp.x, dp.y + d, dp.z)) - distance(vec3(dp.x, dp.y - d, dp.z)),
			distance(vec3(dp.x, dp.y, dp.z + d)) - distance(vec3(dp.x, dp.y, dp.z - d))
		));

		vec3 dirc = normalize(camPos - dp.xyz);

		if (dot(normal, dirc) < dot(-normal, dirc))
			normal = -normal;

		bool direct = false;
		intensity = 0;
		for (int i = 0; i < lights.length(); i++)
		{
			float sh = softShadow(dp.xyz, normalize(lights[i] - dp.xyz), 0.01, length(lights[i] - dp.xyz), lightIntensity);
			if (sh != 0)
			{
				direct = true;
				intensity += map(0, 1, AO, 1, sh * max(dot(normal, normalize(lights[i] - dp.xyz)), 0));
			}
		}
		if (direct)
		{
			intensity = min(intensity, 1);
		}
		else
		{
			intensity = map(0, 1, 0, AO, (1 - pow(min(dp.w / 100, 1), 2)));
		}

		vec3 hsl;
		hsl = vec3(mod(length(dp.xyz) * 2, 3.0), 0.6, 0.8);

		if(coloring)
			return intensity * vec3(hsl2rgb(hsl));
		else
			return vec3(intensity);
	}
	else
	{
		float glowdist = distance(dp.xyz);
		vec3 glowColor = vec3(1, 1, 1);
		if (glowdist < threshold && glow)
		{
			float temp = map(lengthLimit, threshold, 1, 0, glowdist);
			return abs(glowColor - style) * temp + min(glowColor, style);
		}
		else
			return style;
	}
}


void main()
{
	lights[0] = vec3(0, 0, 30);
	lights[1] = vec3(0, -4, 2);
	//lights[1] = camPos;

	if (msaa > 1)
	{
		//pixel width
		vec2 dpxl = vec2(2 / (aspect.x - 1), 2 / (aspect.y - 1));
		vec2 dspxl = dpxl / msaa;

		vec3 avg = vec3(0);
		for (int i = 0; i < msaa; i++)
		{
			for (int j = 0; j < msaa; j++)
			{
				avg += compute(vec2(-dpxl.x * 0.5 + dspxl.x * (0.5 + i), -dpxl.y * 0.5 + dspxl.y * (0.5 + j)));
			}
		}

		color = vec4(avg / (msaa * msaa), 1);
	}
	else
	{
		color = vec4(compute(vec2(0)), 1);
	}
}



































#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

out vec2 pos;

void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0f);
	pos = vec2(position.x, position.y);
}