//
// Created by lovri on 10/24/2024.
//

#ifndef IRGLAB_BSPLINE_H
#define IRGLAB_BSPLINE_H

#include "Renderable.h"
#include "LineStrip.h"

#include <vector>

using namespace std;

class BSpline : public Renderable {
private:
    LineStrip* lineStrip;
public:
    vector<glm::vec3> controlPoints;
    explicit BSpline();
    glm::vec3 Evaluate(float t) const;
    glm::vec3 EvaluateTangent(float t) const;
    void SendToGpu() override;
    void Draw() override;
    void Bind() override;
    ~BSpline() override;
};

#endif //IRGLAB_BSPLINE_H
