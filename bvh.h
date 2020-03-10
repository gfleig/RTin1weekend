#ifndef BVH_H
#define BVH_H

#include "hittable.h"
#include "myrandom.h"

class bvh_node : public hittable {
    public:
    bvh_node() {}
    bvh_node(hittable **l, int n);

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
    virtual bool bounding_box(aabb& box) const;

    hittable *left;
    hittable *right;
    aabb box;
};

bool bvh_node::bounding_box(aabb& b) const {
    b = box;
    return true;
}

int box_x_compare (const void * a, const void * b) {
        aabb box_left, box_right;
        hittable *ah = *(hittable**)a;
        hittable *bh = *(hittable**)b;
        if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                        std::cerr << "no bounding box in bvh_node constructor\n";
        if ( box_left.min().x() - box_right.min().x() < 0.0  )
            return -1;
        else
            return 1;
}

int box_y_compare (const void * a, const void * b)
{
        aabb box_left, box_right;
        hittable *ah = *(hittable**)a;
        hittable *bh = *(hittable**)b;
        if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                        std::cerr << "no bounding box in bvh_node constructor\n";
        if ( box_left.min().y() - box_right.min().y() < 0.0  )
            return -1;
        else
            return 1;
}
int box_z_compare (const void * a, const void * b)
{
        aabb box_left, box_right;
        hittable *ah = *(hittable**)a;
        hittable *bh = *(hittable**)b;
        if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                        std::cerr << "no bounding box in bvh_node constructor\n";
        if ( box_left.min().z() - box_right.min().z() < 0.0  )
            return -1;
        else
            return 1;
}

// original book constructor
/* bvh_node::bvh_node(hittable **l, int n) {
    int axis = int(3 * unif(rng));

    if (axis == 0)
        qsort(l,n, sizeof(hittable *), box_x_compare);
    else if (axis == 1)
        qsort(l,n, sizeof(hittable *), box_y_compare);
    else
        qsort(l,n, sizeof(hittable *), box_z_compare);

    if ( n == 1) {
        left = right = l[0];
    } else if (n == 2) {
        left = l[0];
        right = l[1];
    } else {
        left = new bvh_node(l, n/2);
        right = new bvh_node(l + n/2, n - n/2);
    }

    aabb box_left, box_right;

    if (!left->bounding_box(box_left) || !right->bounding_box(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";

    box = surrounding_box(box_left, box_right);
} */

// peter shirley's blog sah bvh
bvh_node::bvh_node(hittable **l, int n) {
    aabb *boxes = new aabb[n];
    double *left_area = new double[n];
    double *right_area = new double[n];
    aabb main_box;
    bool dummy = l[0]->bounding_box(main_box);
    for (int i = 1; i < n; i++) {
        aabb new_box;
        bool dummy = l[i]->bounding_box(new_box);
        main_box = surrounding_box(new_box, main_box);
    }

    int axis = main_box.longest_axis();
    if (axis == 0)
        qsort(l, n, sizeof(hittable *), box_x_compare);
    else if (axis == 1)
        qsort(l, n, sizeof(hittable *), box_y_compare);
    else
        qsort(l, n, sizeof(hittable *), box_x_compare);

    for (int i = 0; i < n; i++)
        bool dummy = l[i]->bounding_box(boxes[i]);
    
    left_area[0] = boxes[0].area();
    aabb left_box = boxes[0];
    for (int i = 1; i < n - 1; i++) {
        left_box = surrounding_box(left_box, boxes[i]);
        left_area[i] = left_box.area();
    }

    right_area[n-1] = boxes[n-1].area();
    aabb right_box = boxes[n-1];
    for (int i = n -  2; i > 0; i--) {
        right_box = surrounding_box(right_box, boxes[i]);
        right_area[i] = right_box.area();
    }

    double min_SAH = DBL_MAX;
    int min_SAH_idx;
    for (int i = 0; i < n - 1; i++) {
        double SAH = i * left_area[i] + (n - i - 1) * right_area[i + 1];
        if (SAH < min_SAH) {
            min_SAH_idx = i;
            min_SAH = SAH;
        }
    }

    if (min_SAH_idx == 0) 
        left = l[0];
    else
        left = new bvh_node(l, min_SAH_idx + 1);
    if (min_SAH_idx == n - 2)
        right = l[min_SAH_idx + 1];
    else
        right = new bvh_node(l + min_SAH_idx + 1, n - min_SAH_idx - 1);
    
    box = main_box;
}

bool bvh_node::hit(const ray& r, double tmin, double tmax, hit_record& rec) const {
    if(box.hit(r, tmin, tmax)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, tmin, tmax, left_rec);
        bool hit_right = right->hit(r, tmin, tmax, right_rec);

        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;            
        } else if (hit_left) {
            rec = left_rec;
            return true;
        } else if (hit_right) {
            rec = right_rec;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

#endif