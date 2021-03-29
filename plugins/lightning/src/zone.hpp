#pragma once
#include "glm.hpp"

using namespace glm;

struct zone {
    vec3 pointa;
    vec3 pointb;
    bool attractor; // Sets if attractor or deflector.

    zone() :
        pointa(vec3()),
        pointb(vec3()),
        attractor(true)
    {};

    zone(vec3 _pointa, vec3 _pointb, bool _attractor) :
        pointa(_pointa),
        pointb(_pointb),
        attractor(_attractor)
    {};

    ~zone() {}

    bool operator==(const zone& z) const {
        return ((all(equal(pointa, z.pointa)) && all(equal(pointb, z.pointb))) || (all(equal(pointa, z.pointb)) && all(equal(pointb, z.pointa))));
    }
};