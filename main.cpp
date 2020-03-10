#include <iostream>
#include <fstream>
#include <float.h>
#include <chrono>

#include "bvh.h"
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "myrandom.h"
#include "material.h"

vec3 color(const ray& r, hittable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, DBL_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
             return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        double t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

hittable* ball_sea() { 
    hittable **list = new hittable*[2000];

    int idx = 0;
    for (int i = 0; i < 1023; i++) {
        list[idx++] = new sphere(vec3(4.0*unif(rng) - 2.0, 2 * unif(rng) - 2, 4*unif(rng) - 2), 0.25, new lambertian(vec3(unif(rng) * unif(rng), 
                                                                        unif(rng) * unif(rng), 
                                                                        unif(rng) * unif(rng))));
    }

    //return new hittable_list(list,idx);
    return new bvh_node(list, idx);
}

hittable* random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));

    int i = 1;
    for (int a = -6; a < 6; a++) {
        for (int b = -6; b < 6; b++) {
            double choose_mat = unif(rng);
            vec3 center(a+0.9 * unif(rng), 4.0 * unif(rng), b+0.9*unif(rng));
            if (choose_mat < 0.8) {
                list[i++] = new sphere(center, 0.2, new lambertian(vec3(unif(rng) * unif(rng), 
                                                                        unif(rng) * unif(rng), 
                                                                        unif(rng) * unif(rng))));
            }
            else if (choose_mat < 0.95) {
                list[i++] = new sphere(center, 0.2, new metal(vec3( unif(rng) * unif(rng), 
                                                                    unif(rng) * unif(rng), 
                                                                    unif(rng) * unif(rng)),
                                                                    0.5 * unif(rng)));
            }
            else {
                list[i++] = new sphere(center, 0.2, new dielectric(1.5));
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    
    //return new hittable_list(list,i);
    return new bvh_node(list, i);
}

int main() {
    std::ofstream myfile;
    myfile.open("output.ppm");
    int nx = 200;
    int ny = 100;
    int ns = 100;   // samples
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    // rng
    //std::mt19937_64 rng;
    uint16_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    //std::uniform_real_distribution<double> unif(0, 1);      

    /* obj list
    hittable *list[4];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hittable *world = new hittable_list(list, 5); */

    //hittable *world = random_scene();
    hittable *world = ball_sea();
    //hittable *world = bvh_test();
    
    // camera
    /* vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    double dist_to_focus = 10.0;
    double aperture = 0.1; */

    vec3 lookfrom(0,2,0);
    vec3 lookat(0,0,0);
    double dist_to_focus = 1.0;
    double aperture = 0.0;

    camera cam(lookfrom, lookat, vec3(1,0,0), 90,
           float(nx)/float(ny), aperture, dist_to_focus);

    //vec3 col(0, 0, 0);  

    
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);        
            //#pragma omp parallel for              
            for (int s=0; s < ns; s++) {
                double u = double(i + unif(rng)) / double(nx);
                double v = double(j + unif(rng)) / double(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world,0);
            }
            col /= double(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
    return 0;
}