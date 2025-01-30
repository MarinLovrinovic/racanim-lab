
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "Renderer.h"
#include "FPSManager.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Camera.h"
#include "ParticleSystem.h"

Renderer::Renderer(int width, int height) : width(width), height(height){
    glfwInit();
    gladLoadGL();
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Zadatak X", nullptr, nullptr);
    // provjeri je li se uspio napraviti prozor
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // dohvati sve dostupne OpenGL funkcije
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD");
        exit(-1);
    }
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
    glDepthFunc(GL_LESS);

    glClearColor(0.15, 0.1, 0.1, 1); //boja brisanja platna izmedu iscrtavanja dva okvira

    glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

    fpsManager = new FPSManager(window, 60, 1.0, "Zadatak X");
}

Renderer::~Renderer() {
    glfwTerminate();
    delete fpsManager;
}

void Renderer::RegisterRenderable(Object *object) {
    objects.emplace_back(object);
}

void Renderer::RegisterParticleSystem(ParticleSystem* particleSystem) {
    for (auto pg : particleSystem->particleGraphics) {
        objects.emplace_back(pg);
    }
};

#include <algorithm>

bool CompareRenderingOrder(const Object* a, const Object* b) {
    return a->squareDistanceToCamera > b->squareDistanceToCamera;
}

void ComputeDistancesToCamera(std::vector<Object*>& objects, glm::vec3 eyePosition) {
    for (auto object : objects) {
        glm::vec3 pos = object->transforms[0].GetPosition();
        float dx = pos.x - eyePosition.x;
        float dy = pos.y - eyePosition.y;
        float dz = pos.z - eyePosition.z;
        object->squareDistanceToCamera = dx * dx + dy * dy + dz * dz;
    }
}

void Renderer::Render(const Camera &camera, const Light &light) {
    glm::mat4 viewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    glm::vec3 eyePosition = camera.GetPosition();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComputeDistancesToCamera(objects, eyePosition);
    std::sort(objects.begin(), objects.end(), CompareRenderingOrder);

    for (auto object : objects) {
        object->Draw(viewProjectionMatrix, eyePosition, light);
    }
    glfwSwapBuffers(window);
}


