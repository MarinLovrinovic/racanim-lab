#include "ApproximatingBezierCurve.h"

#include <utility>

ApproximatingBezierCurve::ApproximatingBezierCurve() {
    lineStrip = new LineStrip();
}

ApproximatingBezierCurve::~ApproximatingBezierCurve() {
    delete lineStrip;
}

glm::vec3 EvaluateInternal(const vector<glm::vec3> &controlPoints, float t) {
    if (controlPoints.size() == 1)
        return controlPoints[0];
    vector<glm::vec3> interpolatedPoints;
    interpolatedPoints.reserve(controlPoints.size() - 1);
    for (int i = 0; i < controlPoints.size() - 1; i++) {
        interpolatedPoints.emplace_back(glm::mix(controlPoints[i], controlPoints[i + 1], t));
    }
    return EvaluateInternal(interpolatedPoints, t);
}

glm::vec3 ApproximatingBezierCurve::Evaluate(float t) const {
    return EvaluateInternal(controlPoints, t);
}

void ApproximatingBezierCurve::SendToGpu() {
    if (controlPoints.empty()) {
        lineStrip->SendToGpu();
        return;
    }

    vector<glm::vec3> curve;
    for (float t = 0.0f; t < 1.0f; t += 0.01f) {
        curve.emplace_back(Evaluate(t));
    }
    curve.emplace_back(Evaluate(1.0f));
    lineStrip->vertices = curve;
    lineStrip->SendToGpu();
}
void ApproximatingBezierCurve::Draw() {
    lineStrip->Draw();
}
void ApproximatingBezierCurve::Bind() {
    lineStrip->Bind();
}