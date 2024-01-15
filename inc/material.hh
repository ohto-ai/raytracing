#pragma once
// material.hh

#ifndef _OHTOAI_MATERIAL_H_
#define _OHTOAI_MATERIAL_H_

#include "color.hh"
#include "ray.hh"

namespace ohtoai{
    namespace math {
        class HitRecord;
        class Material {
        public:
            virtual bool scatter(const Ray& light, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
            ~Material() = default;
        };

        class Lambertian : public Material {
        public:
            Lambertian(const Color& a) : albedo_(a) {}

            virtual bool scatter(const Ray& light, const HitRecord& rec, Color& attenuation, Ray& scattered) const override {
                auto scatter_direction = rec.normal + random_unit_vector();
                if (scatter_direction.near_zero()) {
                    scatter_direction = rec.normal;
                }
                scattered = make_ray(rec.point, scatter_direction);
                attenuation = albedo_;
                return true;
            }
        protected:
            Color albedo_;
        };

        class Metal : public Material {
        public:
            Metal(const Color& a, real f = 0) : albedo_(a), fuzz_(f < 1 ? f : 1) {}

            virtual bool scatter(const Ray& light, const HitRecord& rec, Color& attenuation, Ray& scattered) const override {
                auto reflected = light.direction().normalized().reflect(rec.normal);
                scattered = make_ray(rec.point, reflected + fuzz_ * random_in_unit_sphere());
                attenuation = albedo_;
                return true;
                // return scattered.direction().dot(rec.normal) > 0;
            }
        protected:
            Color albedo_;
            real fuzz_;
        };
    }
}

#endif // !_OHTOAI_MATERIAL_H_
