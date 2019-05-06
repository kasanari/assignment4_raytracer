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

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1-cosine), 5);
}

bool refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted) {
    glm::vec3 uv = glm::normalize(v);
    float dt = glm::dot(uv, n);
    float discriminant = 1.0f - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv-n*dt) - n*glm::sqrt(discriminant);
        return true;
    }
    else {
        return false;
    }
}

class dielectric : public material { 
    public:
        dielectric(float ri) : ref_idx(ri) {}
        virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const  {
             glm::vec3 outward_normal;
             glm::vec3 reflected = reflect(r_in.direction(), rec.normal);
             float ni_over_nt;
             attenuation = glm::vec3(1.0, 1.0, 1.0); 
             glm::vec3 refracted;
             float reflect_prob;
             float cosine;
             if (glm::dot(r_in.direction(), rec.normal) > 0) {
                  outward_normal = -rec.normal;
                  ni_over_nt = ref_idx;
                  cosine = glm::dot(r_in.direction(), rec.normal) / r_in.direction().length();
                  cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
             }
             else {
                  outward_normal = rec.normal;
                  ni_over_nt = 1.0 / ref_idx;
                  cosine = -glm::dot(r_in.direction(), rec.normal) / r_in.direction().length();
             }
             if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) 
                reflect_prob = schlick(cosine, ref_idx);
             else 
                reflect_prob = 1.0;
             if (drand48() < reflect_prob) 
                scattered = Ray(rec.p, reflected);
             else 
                scattered = Ray(rec.p, refracted);
             return true;
        }

        float ref_idx;
};


} 
