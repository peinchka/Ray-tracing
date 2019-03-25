#ifndef CAMERAH
#define CAMERAH
#include "ray.h"

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0*vec3(float(rand() % 10000) / 10000, float(rand() % 10000) / 10000, 0) - vec3(1, 1, 0);
    } while (dot(p,p) >= 1.0);
    return p;
}

class camera {
    public:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, float t0, float t1) { // vfov is top to bottom in degrees
			time0 = t0;
			time1 = t1;
			lens_radius = aperture / 2;
			float M_PI = 3.141592654f;
			float theta = vfov * M_PI / float(180);
			float half_height = focus_dist * tan(theta / 2);
			float half_width = aspect * half_height;
			ray_origin = lookfrom;
			w = unit_vector(lookfrom - lookat);
			u = unit_vector(cross(vup, w));
			v = cross(w, u);
			lower_left_corner = lookfrom - half_width * u - half_height * v - focus_dist * w;
			horizontal = 2 * half_width*u;
			vertical = 2 * half_height*v;
        }
        ray get_ray(float s, float t) {
            vec3 rd = lens_radius*random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
			float time = time0 + (float(rand() % 10000) / 10000) * (time1 - time0);
			return ray(ray_origin + offset, lower_left_corner + s * horizontal + t * vertical - ray_origin - offset, time);
            //return ray(lookfrom + offset, lookat - lookfrom + (2.0 * s - 1.0) * half_width * u + (2.0 * t - 1.0) * half_height * v - offset);
        }

        vec3 ray_origin;
		vec3 lookat;
		vec3 lookfrom;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
		float time0, time1;
        float lens_radius;
};
#endif




