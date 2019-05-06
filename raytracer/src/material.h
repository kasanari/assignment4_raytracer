#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>
#include "ray.h"
#include "hitable.h"
namespace rt {

struct HitRecord;

class material {
    public:
        virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
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

glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
    return v - 2*glm::dot(v, n)*n;
}

class metal : public material {
    public:
        metal(const glm::vec3& a) : albedo(a) {}

        virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const {
            glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
            scattered = Ray(rec.p, reflected);
            attenuation = albedo;
            return (glm::dot(scattered.direction(), rec.normal) > 0);
        }

        glm::vec3 albedo;
};

} 
