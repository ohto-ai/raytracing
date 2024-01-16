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

        class Dielectric : public Material {
        public:
            Dielectric(real index_of_refraction) : ir_(index_of_refraction) {}

            virtual bool scatter(const Ray& light, const HitRecord& rec, Color& attenuation, Ray& scattered) const override {
                attenuation = Color(1.0, 1.0, 1.0);
                real refraction_ratio = rec.front_face ? (1.0 / ir_) : ir_;

                Vec3 unit_direction = light.direction().normalized();
                real cos_theta = std::min(-unit_direction.dot(rec.normal), 1.0);
                real sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

                bool cannot_refract = refraction_ratio * sin_theta > 1.0;
                Vec3 direction;
                if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_real()) {
                    direction = unit_direction.reflect(rec.normal);
                } else {
                    direction = unit_direction.refract(rec.normal, refraction_ratio);
                }
                scattered = make_ray(rec.point, direction);
                return true;
            }
        protected:
            real ir_;
        protected:
            static real reflectance(real cosine, real ref_idx) {
                // Use Schlick's approximation for reflectance.
                auto r0 = (1 - ref_idx) / (1 + ref_idx);
                r0 *= r0;
                return r0 + (1 - r0) * std::pow((1 - cosine), 5);
            }
        };
    }
}

#endif // !_OHTOAI_MATERIAL_H_
