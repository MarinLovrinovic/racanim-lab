//
// Created by lovri on 1/12/2025.
//

#include "glm/glm.hpp"
#include "Particle.h"
#include "Object.h"
#include <vector>
#include <random>

using namespace std;

#ifndef IRGLAB_PARTICLESYSTEM_H
#define IRGLAB_PARTICLESYSTEM_H

class ParticleSystem {
    int poolSize;
    int nextSpawnIndex = 0;
    float timeSinceLastSpawn = 0;

    vector<Particle> particles;

    void SpawnParticle();

    std::mt19937 gen;
    float Random(float min, float max);
public:
    vector<Object*> particleGraphics;

    float spawnPeriod = 0.2;

    float particleLifetime = 5;
    float particleLifetimeRandom = 2;

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 positionRandom = glm::vec3(1, 0, 1);

    glm::vec3 initialVelocity = glm::vec3(0, 1, 0);
    glm::vec3 initialVelocityRandom = glm::vec3(1, 0, 1);

    glm::vec4 startColor = glm::vec4(1, 1, 0, 1);
    glm::vec4 endColor = glm::vec4(1, 0, 0, 0);

    float startSize = 1;
    float startSizeRandom = 0;
    float endSize = 0;
    float endSizeRandom = 0;

    glm::vec3 gravity = glm::vec3(0, -1, 0);

    ParticleSystem(Renderable *renderable, Shader *shader, int poolSize);
    void Update(float dt, glm::vec3 cameraPosition);
};

#endif //IRGLAB_PARTICLESYSTEM_H
