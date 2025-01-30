//
// Created by lovri on 5/23/2024.
//
#include "InterpolatingBezierCurve.h"

InterpolatingBezierCurve::InterpolatingBezierCurve() {
    approximatingBezierCurve = new ApproximatingBezierCurve();
}

InterpolatingBezierCurve::~InterpolatingBezierCurve() {
    delete approximatingBezierCurve;
}

glm::vec3 InterpolatingBezierCurve::Evaluate(float t) const {
    return approximatingBezierCurve->Evaluate(t);
}

void InterpolatingBezierCurve::SendToGpu() {
    if (controlPoints.size() != 4) {
        approximatingBezierCurve->SendToGpu();
        return;
    }

    glm::mat4x3 p(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);

    glm::mat4 f(1, 0, 0, 0,
                8.0f/27.0f, 12.0f/27.0f, 6.0f/27.0f, 1.0f/27.0f,
                1.0f/27.0f, 6.0f/27.0f, 12.0f/27.0f, 8.0f/27.0f,
                0, 0, 0, 1);

    glm::mat4x3 a = p * glm::inverse(f);
    approximatingBezierCurve->controlPoints = { a[0], a[1], a[2], a[3] };
    approximatingBezierCurve->SendToGpu();
}

void InterpolatingBezierCurve::Draw() {
    approximatingBezierCurve->Draw();
}
void InterpolatingBezierCurve::Bind() {
    approximatingBezierCurve->Bind();
}




