
#include "LineStrip.h"
#include "Renderable.h"

#include <utility>

LineStrip::LineStrip() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

LineStrip::LineStrip(vector<glm::vec3> vertices):
        vertices(std::move(vertices)) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void LineStrip::SendToGpu() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void LineStrip::Bind() {
    glBindVertexArray(VAO);
}

void LineStrip::Draw() {
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
}

LineStrip::~LineStrip() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}