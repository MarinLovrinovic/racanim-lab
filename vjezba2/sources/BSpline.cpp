#include "BSpline.h"
#include <cmath>

BSpline::BSpline() {
    lineStrip = new LineStrip();
}

BSpline::~BSpline() {
    delete lineStrip;
}

glm::vec3 EvaluateSegment(const glm::vec3 controlPoints[], float t) {
    auto r = glm::mat4x3(
            controlPoints[0],
            controlPoints[1],
            controlPoints[2],
            controlPoints[3]);

    auto m = glm::mat4x4(
            -1,  3, -3,  1,
             3, -6,  3,  0,
            -3,  0,  3,  0,
             1,  4,  1,  0);

    float t2 = t * t;
    float t3 = t2 * t;
    auto tVec = glm::vec4(t3, t2, t, 1);

    return r * m * (1.0f/6.0f) * tVec;
}

glm::vec3 BSpline::Evaluate(float t) const {
    float intPart;
    float fracPart = modff(t, &intPart);
    return EvaluateSegment(&controlPoints[(int)intPart], fracPart);
}

glm::vec3 EvaluateSegmentTangent(const glm::vec3 controlPoints[], float t) {
    auto r = glm::mat4x3(
            controlPoints[0],
            controlPoints[1],
            controlPoints[2],
            controlPoints[3]);

    auto m = glm::mat3x4(
            -1,  3, -3,  1,
            2, -4,  2,  0,
            -1,  0,  1,  0);

    auto tVec = glm::vec3(t * t, t, 1);

    return glm::normalize(r * m * (1.0f/2.0f) * tVec);
}

glm::vec3 BSpline::EvaluateTangent(float t) const {
    float intPart;
    float fracPart = modff(t, &intPart);
    return EvaluateSegmentTangent(&controlPoints[(int)intPart], fracPart);
}

void BSpline::SendToGpu() {
    vector<glm::vec3> curve;
    for (int i = 0; i < controlPoints.size() - 3; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.01f) {
            curve.emplace_back(Evaluate(i + t));
        }
    }
    lineStrip->vertices = curve;
    lineStrip->SendToGpu();
}
void BSpline::Draw() {
    lineStrip->Draw();
}
void BSpline::Bind() {
    lineStrip->Bind();
}


