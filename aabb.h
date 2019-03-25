#pragma once

#include "ray.h"
#include "hitable.h"

class aabb {
	public:
		aabb() {}
		aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

		vec3 min() const { return _min; }
		vec3 max() const { return _max; }

		inline bool hit(const ray& r, float tmin, float tmax) const {
			for (int a = 0; a < 3; a++) {
				float invD = 1.0f / r.direction()[a];
				float t0 = (min()[a] - r.ray_origin()[a]) * invD;
				float t1 = (max()[a] - r.ray_origin()[a]) * invD;
				if (invD < 0.0f)
					std::swap(t0, t1);
				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;
				if (tmax <= tmin)
					return false;
			}
			return true;
		}
		
		vec3 _min;
		vec3 _max;
};

