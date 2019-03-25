#ifndef MOVINGSPHEREH
#define MOVINGSPHEREH

#include "hitable.h"

class moving_sphere : public hitable {
public:
	moving_sphere() {}
	moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m) : centre0(cen0), centre1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	vec3 centre(float time) const;
	vec3 centre0, centre1;
	float time0, time1;
	float radius;
	material* mat_ptr;
};

vec3 moving_sphere::centre(float time) const {
	return centre0 + ((time - time0) / (time1 - time0)) * (centre1 - centre0);
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const {
	aabb box0(centre(t0) - vec3(radius, radius, radius), centre(t0) + vec3(radius, radius, radius));
	aabb box1(centre(t1) - vec3(radius, radius, radius), centre(t1) + vec3(radius, radius, radius));
	box = surrounding_box(box0, box1);
	return true;
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 oc = r.ray_origin() - centre(r.time());
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant >= 0.0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - centre(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - centre(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

#endif



