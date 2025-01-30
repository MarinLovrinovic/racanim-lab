// Local Headers


// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

//nekima mozda ne radi primjerASSIMP zbog ponovnih definicija stbi funkcija.
//Jedno od mogucih rjesenja je da se zakomentira linija #define STB_IMAGE_IMPLEMENTATION.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TriangleMesh.h"
#include "Object.h"
#include "FPSManager.h"
#include "Renderer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "Camera.h"
#include "LineStrip.h"
#include "glm/ext/matrix_transform.hpp"
#include "ApproximatingBezierCurve.h"
#include "InterpolatingBezierCurve.h"
#include "Light.h"
#include "BSpline.h"
#include "ParticleSystem.h"
#include <chrono>

#define _USE_MATH_DEFINES

// Standard Headers
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

const double pi = 3.14159265358979323846;

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* LoadShaderWithGeometry(char* path, char* naziv) {
    std::string sPath(path);
    std::string pathVert;
    std::string pathFrag;
    std::string pathGeom;

    pathVert.append(path, sPath.find_last_of("\\/") + 1);
    pathFrag.append(path, sPath.find_last_of("\\/") + 1);
    pathGeom.append(path, sPath.find_last_of("\\/") + 1);
    if (pathFrag[pathFrag.size() - 1] == '/') {
        pathVert.append("shaders/");
        pathFrag.append("shaders/");
        pathGeom.append("shaders/");
    }
    else if (pathFrag[pathFrag.size() - 1] == '\\') {
        pathVert.append("shaders\\");
        pathFrag.append("shaders\\");
        pathGeom.append("shaders\\");
    }
    else {
        std::cerr << "nepoznat format pozicije shadera";
        exit(1);
    }

    pathVert.append(naziv);
    pathVert.append(".vert");
    pathFrag.append(naziv);
    pathFrag.append(".frag");
    pathGeom.append(naziv);
    pathGeom.append(".geom");

    return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
}
Shader* LoadShader(char* path, char* naziv) {
    std::string sPath(path);
    std::string pathVert;
    std::string pathFrag;

    pathVert.append(path, sPath.find_last_of("\\/") + 1);
    pathFrag.append(path, sPath.find_last_of("\\/") + 1);
    if (pathFrag[pathFrag.size() - 1] == '/') {
        pathVert.append("shaders/");
        pathFrag.append("shaders/");
    }
    else if (pathFrag[pathFrag.size() - 1] == '\\') {
        pathVert.append("shaders\\");
        pathFrag.append("shaders\\");
    }
    else {
        std::cerr << "nepoznat format pozicije shadera";
        exit(1);
    }

    pathVert.append(naziv);
    pathVert.append(".vert");
    pathFrag.append(naziv);
    pathFrag.append(".frag");

    return new Shader(pathVert.c_str(), pathFrag.c_str());
}


TriangleMesh* ImportMesh(const string& path) {
    Assimp::Importer importer;

    std::string dirPath(path, 0, path.find_last_of("\\/"));
    std::string resPath(dirPath);
    resPath.append("\\resources"); //za linux pretvoriti u forwardslash
    std::string objPath(resPath);
    objPath.append("\\quad\\quad.obj"); //za linux pretvoriti u forwardslash

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType |
                                             aiProcess_FlipUVs |
                                             aiProcess_GenNormals
    );
    if (!scene) {
        cerr << importer.GetErrorString();
        exit(-1);
    }
    if (!scene->HasMeshes()){
        cerr << "Scene has no meshes." << endl;
        exit(-1);
    }
    aiMesh* mesh = scene->mMeshes[0];

    vector<glm::vec3> vertices;
    for (int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = mesh->mVertices[i];
        vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }
    
    vector<glm::vec3> normals;
    if (mesh->mNormals != nullptr){
        for (int i = 0; i < mesh->mNumVertices; i++) {
            auto normal = mesh->mNormals[i];
            normals.emplace_back(normal.x, normal.y, normal.z);
        }
    }

    vector<glm::vec3> uvCoords;
    for (int i = 0; i < mesh->mNumVertices; i++) {
        auto uv = mesh->mTextureCoords[0][i];
        uvCoords.emplace_back(uv.x, uv.y, uv.z);
    }

    vector<int> indices;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        if (face.mNumIndices != 3) {
            std::cerr << "Error: Only triangular faces are supported" << std::endl;
            exit(-1);
        }
        for (int j = 0; j < face.mNumIndices; j++){
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return new TriangleMesh(vertices, normals, indices, uvCoords);
}

Material* ImportMaterial(const string& path) {
    Assimp::Importer importer;

    std::string dirPath(path, 0, path.find_last_of("\\/"));
    std::string resPath(dirPath);
    resPath.append("\\resources"); //za linux pretvoriti u forwardslash
    std::string objPath(resPath);
    objPath.append("\\glava\\glava.obj"); //za linux pretvoriti u forwardslash

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType |
                                             aiProcess_FlipUVs |
                                             aiProcess_GenNormals

    );

    if (!scene) {
        cerr << importer.GetErrorString();
        exit(-1);
    }
    if (!scene->HasMaterials()) {
        cerr << "Scene has no materials." << endl;
        exit(-1);
    }
    aiMaterial* material = scene->mMaterials[scene->mNumMaterials - 1];

    aiColor3D ambientK, diffuseK, specularK;
    auto* output = new Material;

    material->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
    output->ambient = glm::vec3(ambientK.r, ambientK.g, ambientK.g);

    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
    output->diffuse = glm::vec4(diffuseK.r, diffuseK.g, diffuseK.g, 1);

    material->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
    output->specular = glm::vec3(specularK.r, specularK.g, specularK.g);

    material->Get(AI_MATKEY_SHININESS, output->shininess);
    return output;
}

Renderer* renderer;

void FramebufferSizeCallback(GLFWwindow* window, int Width, int Height)
{
    renderer->width = Width;
    renderer->height = Height;
    glViewport(0, 0, Width, Height);
}

Camera camera;
double pitch;
double yaw;

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 newPos(xpos, ypos);
    auto delta = newPos - glm::vec2(renderer->width * 0.5, renderer->height * 0.5);
    delta *= -0.001;

    pitch += delta.y;
    pitch = std::max(-pi * 0.5 + 0.01, std::min(pitch, pi * 0.5 - 0.01));

    yaw += delta.x;

    camera.SetRotation(yaw, glm::vec3(0, 1, 0));
    camera.Rotate(pitch, camera.LocalToGlobalDir() * glm::vec4(1, 0, 0, 0));
}

LineStrip* controlPolygon;
ApproximatingBezierCurve* approximatingBezierCurve;
BSpline* bSpline;
InterpolatingBezierCurve* interpolatingBezierCurve;

void PlacePoint() {
    glm::vec3 point = camera.GetPosition() - 4.0f * camera.GetLocalZ();
    controlPolygon->vertices.emplace_back(point);
    controlPolygon->SendToGpu();

    approximatingBezierCurve->controlPoints.emplace_back(point);
    approximatingBezierCurve->SendToGpu();

//    bSpline->controlPoints.emplace_back(point + glm::vec3(1, 0, 0));
//    bSpline->SendToGpu();

    interpolatingBezierCurve->controlPoints.emplace_back(point);
    if (interpolatingBezierCurve->controlPoints.size() > 4)
        interpolatingBezierCurve->controlPoints.erase(interpolatingBezierCurve->controlPoints.begin());
    interpolatingBezierCurve->SendToGpu();
}

bool forwardPressed;
bool backPressed;
bool leftPressed;
bool rightPressed;
bool upPressed;
bool downPressed;

glm::vec3 moveVector;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT) return;

    bool pressed = action == GLFW_PRESS;
    if (key == GLFW_KEY_W) {
        forwardPressed = pressed;
    } else if (key == GLFW_KEY_A) {
        leftPressed = pressed;
    } else if (key == GLFW_KEY_S) {
        backPressed = pressed;
    } else if (key == GLFW_KEY_D) {
        rightPressed = pressed;
    } else if (key == GLFW_KEY_E) {
        downPressed = pressed;
    } else if (key == GLFW_KEY_Q) {
        upPressed = pressed;
    } else if (pressed && key == GLFW_KEY_F) {
        PlacePoint();
    }
    else {
        return;
    }
    moveVector = glm::vec3((rightPressed ? 1.0f : 0.0f) + (leftPressed ? -1.0f : 0.0f),
                           (upPressed ? 1.0f : 0.0f) + (downPressed ? -1.0f : 0.0f),
                           (backPressed ? 1.0f : 0.0f) + (forwardPressed ? -1.0f : 0.0f));
}

int main(int argc, char* argv[]) {
    renderer = new Renderer(500, 500);

    glfwSetFramebufferSizeCallback(renderer->window, FramebufferSizeCallback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
    glfwSetCursorPosCallback(renderer->window, CursorPosCallback);
    glfwSetKeyCallback(renderer->window, KeyCallback);
    glfwSetInputMode(renderer->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto* triangleMesh = ImportMesh(string(argv[0]));
    triangleMesh->Normalize();
    auto* shader = LoadShader(argv[0], "scene");

//    auto* object = new Object(triangleMesh, LoadShader(argv[0], "scene"));
//    object->SendToGpu();
//    object->material = ImportMaterial(string(argv[0]));
//    renderer->RegisterRenderable(object);

//    auto* object2 = new Object(triangleMesh, LoadShader(argv[0], "scene"));
//    object2->SendToGpu();
//    Transform transform;
//    transform.SetPosition(glm::vec3(3, 0, 0));
//    transform.SetRotation(1.5, glm::normalize(glm::vec3(1, 1, 1)));
//    transform.SetScale(glm::vec3(3, 2, 1));
//    object2->transforms[0] = transform;
//    object2->material->diffuse = glm::vec3(1, 0, 0);
//    object2->material->specular = glm::vec3(0, 0, 1);
//    renderer->RegisterRenderable(object2);

    controlPolygon = new LineStrip();
    auto* controlPolygonObject = new Object(controlPolygon, LoadShader(argv[0], "line"));
    controlPolygonObject->SendToGpu();
    controlPolygonObject->material->diffuse = glm::vec4(1, 0, 0, 1);
    renderer->RegisterRenderable(controlPolygonObject);

    approximatingBezierCurve = new ApproximatingBezierCurve();
    auto* approximatingBezierObject =
            new Object(approximatingBezierCurve,LoadShader(argv[0], "line"));
    approximatingBezierObject->SendToGpu();
    approximatingBezierObject->material->diffuse = glm::vec4(1, 1, 0, 1);
    renderer->RegisterRenderable(approximatingBezierObject);

//    auto* movingObject = new Object(triangleMesh, LoadShader(argv[0], "scene"));
//    movingObject->SendToGpu();
//    movingObject->material = ImportMaterial(string(argv[0]));
//    renderer->RegisterRenderable(movingObject);

    bSpline = new BSpline();
    bSpline->controlPoints = {
            glm::vec3(0, 0, 0),    // V1
            glm::vec3(0, 10, 5),   // V2
            glm::vec3(10, 10, 10), // V3
            glm::vec3(10, 0, 15),  // V4
            glm::vec3(0, 0, 20),   // V5
            glm::vec3(0, 10, 25),  // V6
            glm::vec3(10, 10, 30), // V7
            glm::vec3(10, 0, 35),  // V8
            glm::vec3(0, 0, 40),   // V9
            glm::vec3(0, 10, 45),  // V10
            glm::vec3(10, 10, 50), // V11
            glm::vec3(10, 0, 55)   // V12
    };
    auto* bSplineObject = new Object(bSpline,LoadShader(argv[0], "line"));
    bSplineObject->SendToGpu();
    bSplineObject->material->diffuse = glm::vec4(0, 0, 1, 1);
    renderer->RegisterRenderable(bSplineObject);

    interpolatingBezierCurve = new InterpolatingBezierCurve();
    auto* interpolatingBezierObject =
            new Object(interpolatingBezierCurve,LoadShader(argv[0], "line"));
    interpolatingBezierObject->SendToGpu();
    interpolatingBezierObject->material->diffuse = glm::vec4(0, 1, 1, 1);
    renderer->RegisterRenderable(interpolatingBezierObject);

    camera.SetPosition(glm::vec3(0, 0, 10));

    Light light;
    light.SetPosition(glm::vec3(10, 10, 10));
    light.ambient = glm::vec3(1, 1, 1);

    ParticleSystem particleSystem(triangleMesh, shader, 100);
    particleSystem.spawnPeriod = 0.2;
    particleSystem.particleLifetime = 5;
    particleSystem.particleLifetimeRandom = 2;
    particleSystem.position = glm::vec3(0, 0, 0);
    particleSystem.positionRandom = glm::vec3(1, 0, 1);
    particleSystem.initialVelocity = glm::vec3(0, 1, 0);
    particleSystem.initialVelocityRandom = glm::vec3(1, 0, 1);
    particleSystem.startColor = glm::vec4(1, 1, 0, 1);
    particleSystem.endColor = glm::vec4(1, 0, 0, 0);
    particleSystem.startSize = 1;
    particleSystem.startSizeRandom = 0;
    particleSystem.endSize = 3;
    particleSystem.endSizeRandom = 0.5;
    particleSystem.gravity = glm::vec3(0, -1, 0);

    renderer->RegisterParticleSystem(&particleSystem);

    ParticleSystem particleSystem2(triangleMesh, shader, 100);
    particleSystem2.spawnPeriod = 0.1;
    particleSystem2.particleLifetime = 2;
    particleSystem2.particleLifetimeRandom = 1;
    particleSystem2.position = glm::vec3(0, 0, 0);
    particleSystem2.positionRandom = glm::vec3(1, 0, 1);
    particleSystem2.initialVelocity = glm::vec3(0, 0, 0);
    particleSystem2.initialVelocityRandom = glm::vec3(1, 1, 1);
    particleSystem2.startColor = glm::vec4(0, 1, 0, 1);
    particleSystem2.endColor = glm::vec4(0, 0, 1, 1);
    particleSystem2.startSize = 0.7;
    particleSystem2.startSizeRandom = 0.1;
    particleSystem2.endSize = 0;
    particleSystem2.endSizeRandom = 0;
    particleSystem2.gravity = glm::vec3(0, -1, 0);

    renderer->RegisterParticleSystem(&particleSystem2);

    float dt = 0.005f;
    float t = 0.0f;
    auto previousTime = std::chrono::steady_clock::now();
    while (glfwWindowShouldClose(renderer->window) == false) {
        if (moveVector != glm::vec3(0.0f)) {
            camera.Move(dt * camera.LocalToGlobalDir() * glm::vec4(moveVector, 0.0f));
        }

//        Transform& movingTransform = movingObject->transforms[0];
//        movingTransform.SetPosition(bSpline->Evaluate(t));
//        movingTransform.localZ = glm::vec4(bSpline->EvaluateTangent(t), 0);
//        movingTransform.localX = glm::vec4(glm::normalize(glm::cross(glm::vec3(0, 1, 0), glm::vec3(movingTransform.localZ))), 0);
//        movingTransform.localY = glm::vec4(glm::cross(glm::vec3(movingTransform.localZ), glm::vec3(movingTransform.localX)), 0);

        t += 0.02f * dt;
        if (t >= bSpline->controlPoints.size() - 3)
            t = 0.0f;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        previousTime = currentTime;


        particleSystem.position = bSpline->Evaluate(t);
        particleSystem.Update(deltaTime.count(), camera.GetPosition());
        particleSystem2.Update(deltaTime.count(), camera.GetPosition());

//        object2->transforms[0].LookAt(camera.GetPosition());

        renderer->Render(camera, light);

        glfwPollEvents();
        if (glfwGetKey(renderer->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(renderer->window, true);
        glfwSetCursorPos(renderer->window, renderer->width * 0.5, renderer->height * 0.5);

    }
    delete renderer;
    return EXIT_SUCCESS;
}
