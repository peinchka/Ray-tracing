#pragma once

#include <iostream>
#include <fstream>
#include "perlin.h"
using namespace std;

class texture {
	public:
		virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
	public:
		constant_texture() {}
		constant_texture(vec3 c) : colour(c) {}
		virtual vec3 value(float u, float v, const vec3& p) const {
			return colour;
		}
		vec3 colour;
};

class checker_texture : public texture {
	public:
		checker_texture() {}
		checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) {}
		virtual vec3 value(float u, float v, const vec3& p) const {
			//float sines = sin(0.1 * p.x()) * sin(0.1 * p.y()) * sin(0.1 * p.z());
			float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
			if (sines < 0) {
				return odd->value(u, v, p);
			}
			else {
				return even->value(u, v, p);
			}	
		}
		texture* odd;
		texture* even;
};

class noise_texture : public texture {
public:
	noise_texture() {}
	noise_texture(vec3 col1, vec3 col2, float sc, int pat) : colour1(col1), colour2(col2), scale(sc), pattern(pat) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		if (pattern == 1) {
			return colour1 * 0.5*(1 + sin(scale*p.x() + 10 * noise.turb(p))) + colour2 * 0.5*(1 - sin(scale*p.x() + 10 * noise.turb(p)));
		}
		else if (pattern == 2) {
			return colour1 * (sin(scale*p.x() + 10 * noise.turb(p)))*(sin(scale*p.x() + 10 * noise.turb(p))) + colour2 * (cos(scale*p.x() + 10 * noise.turb(p)))*(cos(scale*p.x() + 10 * noise.turb(p)));
		}
		else if (pattern == 3) {
			return colour1 * noise.turb(scale * p) + colour2 * (1 - noise.turb(scale * p));
		}
		else if (pattern == 4) {
			return colour1 * 0.5*(1 + noise.turb(scale * p));
		}
		else if (pattern == 5) {
			return colour1 * noise.noise(scale * p);
		}
	}
	perlin noise;
	vec3 colour1, colour2;
	float scale;
	int pattern;
};

class image_texture : public texture {
public:
	image_texture() {}
	image_texture(unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) {}
	virtual vec3 value(float u, float v, const vec3& p) const;
	unsigned char* data;
	int nx, ny;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
	int i = int((u)*nx);
	int j = int((1 - v)*ny - 0.001);
//	cout << u << ", " << v << endl;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > nx - 1) i = nx - 1;
	if (j > ny - 1) j = ny - 1;
	float r = int(data[3 * i + 3 * nx*j]) / 255.0;
//	cout << data[3 * i + 3 * nx*j] << endl << data[3 * i + 3 * nx*j+1] << endl << data[3 * i + 3 * nx*j+2];
	float g = int(data[3 * i + 3 * nx*j + 1]) / 255.0f;
	float b = int(data[3 * i + 3 * nx*j + 2]) / 255.0f;
	return vec3(r, g, b);
}
