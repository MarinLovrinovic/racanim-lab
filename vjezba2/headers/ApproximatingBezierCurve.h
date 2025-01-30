#ifndef IRGLAB_APPROXIMATINGBEZIERCURVE_H
#define IRGLAB_APPROXIMATINGBEZIERCURVE_H

#include "Renderable.h"
#include "LineStrip.h"

#include <vector>

using namespace std;

class ApproximatingBezierCurve : public Renderable {
private:
    LineStrip* lineStrip;
public:
    vector<glm::vec3> controlPoints;
    explicit ApproximatingBezierCurve();
    glm::vec3 Evaluate(float t) const;
    void SendToGpu() override;
    void Draw() override;
    void Bind() override;
    ~ApproximatingBezierCurve() override;
};

#endif //IRGLAB_APPROXIMATINGBEZIERCURVE_H
