#ifndef IRGLAB_RENDERER_H
#define IRGLAB_RENDERER_H

#include "GLFW/glfw3.h"
#include "FPSManager.h"
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"

class Renderer {
private:
    vector<Object*> objects;
public:
    int width;
    int height;
    GLFWwindow* window;
    FPSManager* fpsManager;
    Renderer(int width, int height);
    ~Renderer();
    void RegisterRenderable(Object* object);
    void RegisterParticleSystem(ParticleSystem* particleSystem);
    void Render(const Camera &camera, const Light &light);
};

#endif //IRGLAB_RENDERER_H
