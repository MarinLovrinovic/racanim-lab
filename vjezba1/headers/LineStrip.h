#ifndef IRGLAB_LINESTRIP_H
#define IRGLAB_LINESTRIP_H

#include "Renderable.h"

#include <vector>

using namespace std;

class LineStrip : public Renderable {
private:
    GLuint VBO;
    GLuint VAO;
public:
    vector<glm::vec3> vertices;
    LineStrip();
    explicit LineStrip(vector<glm::vec3> vertices);
    ~LineStrip() override;
    void SendToGpu() override;
    void Draw() override;
    void Bind() override;
};

#endif //IRGLAB_LINESTRIP_H
