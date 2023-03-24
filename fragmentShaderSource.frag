#version 450 core
layout (location = 0) out vec4 color;
uniform float xPivot;
uniform float yPivot;
uniform vec2 constant;
uniform vec2 pixels;
uniform float scale;
uniform int max_iteration;
uniform float exponent;
uniform bool log_expression;

//float normSquare(vec2 zn)
//{
//    return zn.x*zn.x + zn.y*zn.y;
//}

vec2 complexToExponential(vec2 zn)
{
    float r = sqrt(zn.x*zn.x + zn.y*zn.y);
    float w = atan(zn.y, zn.x);
    return vec2(r, w);
}

//vec2 computeNext(vec2 zn)
vec2 computeNext(vec2 zn_ex)
{
    // Zn^2 + C
//    return vec2(zn.x*zn.x - zn.y*zn.y, zn.x*zn.y*2) + constant;
    zn_ex.x = pow(zn_ex.x, exponent);
    zn_ex.y *= exponent;
    vec2 zn = vec2(zn_ex.x*cos(zn_ex.y), zn_ex.x*sin(zn_ex.y));
    zn += constant;

    return complexToExponential(zn);
}

float computeIterations(vec2 z0)
{
//    vec2 zn = z0;
//    int iteration = 0;
//    while((normSquare(zn) < 4.0f) && (iteration < max_iteration))
//    {
//        zn = computeNext(zn);
//        ++iteration;
//    }
//    const float norm = sqrt(normSquare(zn));
//    const float floatIteration = float(iteration) - log2(max(1.0f, log2(norm)));
//    return floatIteration;
    vec2 zn_ex = complexToExponential(z0);
    int iteration = 0;
    while ((zn_ex.x < 1000.0f) && (iteration < max_iteration))
    {
        zn_ex = computeNext(zn_ex);
        ++iteration;
    }
    const float norm = zn_ex.x;
    const float floatIteration = float(iteration) - log2(max(1.0f, log2(norm)));
    return floatIteration;
}

float getColor(float iteration, int maxIteration)
{
    return iteration/maxIteration;
}

void main() {
    const float x = gl_FragCoord.x;
    const float y = pixels.y - gl_FragCoord.y; // float(pixels.y) -
    const float px = (x - xPivot) * scale;
    const float py = (y - yPivot) * scale;
    // compute color
    const float iterations = computeIterations(vec2(px, py));
    if (log_expression)
    {
        float intensity = getColor(iterations, max_iteration) + 0.0001f;
        float w = max(0.0f, -log2(intensity));
        w -= int(w);
        color = vec4(w, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        color = vec4(getColor(iterations, max_iteration), 0.0f, 0.0f, 1.0f);
    }
//    if (y > yPivot) color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    else color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
