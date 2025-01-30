#ifndef IRGLAB_PARTICLE_H
#define IRGLAB_PARTICLE_H

#include "glm/glm.hpp"

class Particle {
public:
    bool active = false;
    float lifetime;
    float remainingLifetime = 1;
    glm::vec3 position = glm::vec3(100, 0, 0);
    glm::vec3 velocity;
    float startSize;
    float endSize;
};

#endif //IRGLAB_PARTICLE_H
