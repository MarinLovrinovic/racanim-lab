#ifndef IRGLAB_INTERPOLATINGBEZIERCURVE_H
#define IRGLAB_INTERPOLATINGBEZIERCURVE_H

#include "Renderable.h"
#include "ApproximatingBezierCurve.h"



class InterpolatingBezierCurve : public Renderable {
private:
    ApproximatingBezierCurve* approximatingBezierCurve;
public:
    vector<glm::vec3> controlPoints;
    explicit InterpolatingBezierCurve();
    glm::vec3 Evaluate(float t) const;
    void SendToGpu() override;
    void Draw() override;
    void Bind() override;
    ~InterpolatingBezierCurve() override;
};

#endif //IRGLAB_INTERPOLATINGBEZIERCURVE_H
