#include "rtweekend.h"

#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include<iostream>

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0) return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    return color(1.0, 1.0, 1.0);
}

int main() {
    const auto aspect_ratio = 1;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // World

    hittable_list world;
    auto distance_to_sun = -1.496e11;
    // * 3 for better visibility
    auto sun_radius = 695700e3 * 30;
    auto distance_to_moon = -3.84402e8;
    // * 3 for better visibility
    auto moon_radius = 1.7374e6 * 25;
    auto earth_radius = 6.3781e6;

    // place immovable sun at (0,0)
    auto sun_position = point3(0, 0, 0);
    auto sun = make_shared<sphere>(
        sun_position, sun_radius, make_shared<lambertian>(color(0.8, 0.6, 0.2)));

    // plane of earth's revolution around sun is x-y plane
    auto earth_position = point3(distance_to_sun * cos(M_PI/4), distance_to_sun * sin(M_PI/4), 0);

    auto earth = make_shared<sphere>(
        earth_position, earth_radius, make_shared<lambertian>(color(0.0, 0.0, 1.0)));

    // TODO 5* inclination of moon's revolution around earth to the ecliptic
    auto moon_position = point3(distance_to_moon * cos(-M_PI/2), distance_to_moon * sin(-M_PI/2), 1000000000) + earth_position;


    auto moon = make_shared<sphere>(
        moon_position, moon_radius, make_shared<lambertian>(color(0.8, 0.8, 0.8)));
    world.add(earth);
    world.add(moon);
    world.add(sun);

    // Camera
    // cam should be on surface of earth or moon, pointed at sun or other body.
    auto lat = (earth_radius+3)*sin(-M_PI/7);
    auto lon = (earth_radius+3)*cos(-M_PI/7);
    camera cam(earth_position + point3(lat, lon, 0), sun_position, vec3(0,1,0), 160, aspect_ratio);

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
