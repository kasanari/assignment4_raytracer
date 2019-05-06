#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

namespace rt {

class material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    material *mat_ptr;
};


class material {
    public:
        virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
};

class Hitable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

glm::vec3 random_in_unit_sphere() {
    glm::vec3 p;
    do {
        p = 2.0f*glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1,1,1);
    } while (glm::length(p) >= 1.0);
    return p;
}

class lambertian : public material {
    public:
        lambertian(const glm::vec3& a) : albedo(a) {}

        virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const {
            glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = Ray(rec.p, target-rec.p);
            attenuation = albedo;
            return true;
        }

        glm::vec3 albedo;
};

} // namespace rt
