#ifndef AABB_H
#define AABB_H

#include "hittable.h"
#include "ray.h"

inline double ffmin(double a, double b) { return a < b? a : b;} 
inline double ffmax(double a, double b) { return a > b? a : b;} 

class aabb {
    public:
    aabb() {}
    aabb(const vec3& a, const vec3& b) {_min = a; _max = b;}

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    // new version of hit function
    bool hit( const ray &r, double tmin, double tmax) const {
        for (int a = 0; a < 3; a++) { // for each axis
            double invD = 1.0f / r.direction()[a];
            double t0 = (min()[a] - r.origin()[a]) * invD;
            double t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);

            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

    /* 
    // old version of hit function
    bool hit( const ray &r, double tmin, double tmax) const {
        for (int a = 0; a < 3; a++) { // for each axis
            double t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
                              (_max[a] - r.origin()[a]) / r.direction()[a]);
            double t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
                              (_max[a] - r.origin()[a]) / r.direction()[a]);

            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    } */
    
    double area() const {
        double a = _max.x() - _min.x();
        double b = _max.y() - _min.y();
        double c = _max.z() - _min.z();
        return 2 * (a*b + a*c + b*c);
    }

    double longest_axis() const {
        double a = _max.x() - _min.x();
        double b = _max.y() - _min.y();
        double c = _max.z() - _min.z();

        if (a>b && a>c)
            return 0;
        else if (b > c)
            return 1;
        else 
            return 2;
    }

    
    vec3 _min;
    vec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 min_corner(ffmin(box0.min().x(), box1.min().x()),
                    ffmin(box0.min().y(), box1.min().y()),
                    ffmin(box0.min().z(), box1.min().z()));
    vec3 max_corner(ffmax(box0.max().x(), box1.max().x()),
                    ffmax(box0.max().y(), box1.max().y()),
                    ffmax(box0.max().z(), box1.max().z()));
    return aabb(min_corner, max_corner);
}


#endif