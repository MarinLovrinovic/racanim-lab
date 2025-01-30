//
// Created by lovri on 1/12/2025.
//
#include "ParticleSystem.h"

#include <utility>
#include <random>

// this is gonna have to be a list of objects because we need to be able to change colors independently
ParticleSystem::ParticleSystem(Renderable *renderable, Shader *shader, int poolSize) {
    this->poolSize = poolSize;
    particles = vector<Particle>(poolSize);
    particleGraphics = vector<Object*>(poolSize);
    for (int i = 0; i < poolSize; i++) {
        particleGraphics[i] = new Object(renderable, shader);
        particleGraphics[i]->transforms[0].SetPosition(particles[i].position);
        particleGraphics[i]->material->diffuse = glm::vec4(0);
        particles[i].position = glm::vec3(1000);
        particleGraphics[i]->transforms[0].SetPosition(particles[i].position);
        particleGraphics[i]->SendToGpu();
    }

    std::random_device rd;
    gen = std::mt19937(rd());
}

void ParticleSystem::Update(float dt, glm::vec3 cameraPosition) {
    timeSinceLastSpawn += dt;
    while (timeSinceLastSpawn >= spawnPeriod) {
        SpawnParticle();
        timeSinceLastSpawn -= spawnPeriod;
    }

    for (int i = 0; i < poolSize; i++) {
        Particle& particle = particles[i];
        if (!particle.active) continue;
        Object* particleGraphic = particleGraphics[i];

        particle.velocity += gravity * dt;
        particle.position += particle.velocity * dt;
        particleGraphic->transforms[0].SetPosition(particle.position);
        particleGraphic->transforms[0].LookAt(cameraPosition);

        particle.remainingLifetime -= dt;
        float lifePartRemaining = particle.remainingLifetime / particle.lifetime;

        particleGraphic->transforms[0]
        .SetScale(glm::vec3(glm::mix(particle.endSize, particle.startSize, lifePartRemaining)));

        particleGraphic->material->diffuse = glm::mix(endColor, startColor, lifePartRemaining);

        if (particle.remainingLifetime <= 0) {
            particle.active = false;
            particleGraphic->material->diffuse = glm::vec4(0);
            particle.position = glm::vec3(1000);
            particleGraphic->transforms[0].SetPosition(particle.position);
        }
    }
}

void ParticleSystem::SpawnParticle() {
    Particle& particle = particles[nextSpawnIndex];
    particle.active = true;
    particleGraphics[nextSpawnIndex]->material->diffuse = startColor;
    nextSpawnIndex++;
    nextSpawnIndex %= poolSize;

    float lifetime = particleLifetime + Random(-particleLifetimeRandom, particleLifetimeRandom);
    particle.lifetime = lifetime;
    particle.remainingLifetime = lifetime;
    particle.position = position + glm::vec3(
            Random(-positionRandom.x, positionRandom.x),
            Random(-positionRandom.y, positionRandom.y),
            Random(-positionRandom.z, positionRandom.z));
    particle.velocity = initialVelocity + glm::vec3(
            Random(-initialVelocityRandom.x, initialVelocityRandom.x),
            Random(-initialVelocityRandom.y, initialVelocityRandom.y),
            Random(-initialVelocityRandom.z, initialVelocityRandom.z));
    particle.startSize = startSize + Random(-startSizeRandom, startSizeRandom);
    particle.endSize = endSize + Random(-endSizeRandom, endSizeRandom);
}

float ParticleSystem::Random(float min, float max) {
    return std::uniform_real_distribution<float>(min, max)(gen);
}


