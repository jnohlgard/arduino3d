#include <arduino3d.h>
//~ #include <math.h>

using Array::Matrix4f;
using Array::Vector4f;
typedef Framebuffer::KS0108Framebuffer FBDev;

Matrix4f identity;
Matrix4f ProjectionMatrix;
Matrix4f ModelWorldMatrix;
Matrix4f WorldViewMatrix;
Matrix4f RotationMatrix;
Matrix4f RotationOffsetMatrix;
Matrix4f ScaleMatrix;
Matrix4f MoveMatrix;
Matrix4f RotationXMatrix;
Matrix4f RotationYMatrix;
Matrix4f RotationZMatrix;

Vector4f cube_vertices[8];
uint8_t cube_indices[24];
static const uint8_t num_cubes = 5;

FBDev fb(128, 64);

void initCube(void)
{
    int i=0;
    for (int z = -1; z < 2; z += 2)
    {
        for (int y = -1; y < 2; y += 2)
        {
            for (int x = -1; x < 2; x += 2)
            {
                cube_vertices[i][0] = static_cast<float>(x);
                cube_vertices[i][1] = static_cast<float>(y);
                cube_vertices[i][2] = static_cast<float>(z);
                cube_vertices[i][3] = 1.0f; // homogenous coordinate
                ++i;
            }
        }
    }
    i = 0;
    // -z
    cube_indices[i++] = 0;
    cube_indices[i++] = 2;
    cube_indices[i++] = 3;
    cube_indices[i++] = 1;
    // +z
    cube_indices[i++] = 4;
    cube_indices[i++] = 5;
    cube_indices[i++] = 7;
    cube_indices[i++] = 6;
    // -y
    cube_indices[i++] = 0;
    cube_indices[i++] = 1;
    cube_indices[i++] = 5;
    cube_indices[i++] = 4;
    // +y
    cube_indices[i++] = 2;
    cube_indices[i++] = 6;
    cube_indices[i++] = 7;
    cube_indices[i++] = 3;
    // -x
    cube_indices[i++] = 0;
    cube_indices[i++] = 4;
    cube_indices[i++] = 6;
    cube_indices[i++] = 2;
    // +x
    cube_indices[i++] = 1;
    cube_indices[i++] = 3;
    cube_indices[i++] = 7;
    cube_indices[i++] = 5;
}

void initModelView(void)
{
    A3D::Identity(ModelWorldMatrix);
    A3D::Translate(WorldViewMatrix, 0.0f, 0.0f, -6.0f); // move cube away from camera
    A3D::Identity(ScaleMatrix);
    A3D::Translate(MoveMatrix, 0.0f, 0.0f, -3.0f); // move cube away from camera
}

void initRotationStep(void)
{
    static const float xstep = 1.0f/17.0f;
    static const float ystep = 1.0f/61.0f;
    static const float zstep = 1.0f/37.0f;
    static const float rotoffset = TWO_PI/static_cast<float>(num_cubes);

    A3D::RotateXrad(RotationXMatrix, xstep);
    A3D::RotateYrad(RotationYMatrix, ystep);
    A3D::RotateZrad(RotationZMatrix, zstep);
    A3D::RotateYrad(RotationOffsetMatrix, rotoffset);

    RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
}

void animateCube(void)
{
    ModelWorldMatrix *= RotationMatrix;
    ScaleMatrix[0] = ScaleMatrix[5] = ScaleMatrix[10] = (2.0 + sin(millis()/1000.0f)) * 0.3;

    MoveMatrix = RotationYMatrix * MoveMatrix;
}

void drawCube(uint8_t index)
{
    // Calculate transform: Model to world to view to projection to device
    Matrix4f SystemMatrix = MoveMatrix;
    for (uint8_t i = 0; i < index; ++i)
    {
        // Offset each cube from the others
        SystemMatrix = RotationOffsetMatrix * SystemMatrix;
    }
    Matrix4f total_transform = ProjectionMatrix * WorldViewMatrix * SystemMatrix * ScaleMatrix * ModelWorldMatrix;
    // Draw quads
    Vector4f transformed_verts[8];
    for (uint8_t i = 0; i < 8; ++i)
    {
        transformed_verts[i] = total_transform * cube_vertices[i];
        transformed_verts[i] /= transformed_verts[i][3]; // Perspective divide
        //~ Serial.print("Vertex ");
        //~ Serial.println(i);
        //~ printVector4f(transformed_verts[i]);
    }

    float display_scale_x = static_cast<float>(128/2);
    float display_scale_y = static_cast<float>(64/2);
    for (uint8_t side = 0; side < sizeof(cube_indices) / 4; ++side)
    {
        Vector4f* v1;
        Vector4f* v2;
        uint8_t x1, y1, x2, y2;
        v1 = &transformed_verts[cube_indices[side*4]];
        for (uint8_t i = 1; i < 4; ++i)
        {
            v2 = &transformed_verts[cube_indices[side*4 + i]];
            // Device transformation
            x1 = static_cast<uint8_t>(fma((*v1)[0], display_scale_x, display_scale_x));
            x2 = static_cast<uint8_t>(fma((*v2)[0], display_scale_x, display_scale_x));
            y1 = static_cast<uint8_t>(fma((*v1)[1], display_scale_y, display_scale_y));
            y2 = static_cast<uint8_t>(fma((*v2)[1], display_scale_y, display_scale_y));
            fb.line(x1, y1, x2, y2, true);
            v1 = v2;
        }
        v1 = &transformed_verts[cube_indices[side*4]];
        x1 = static_cast<uint8_t>(fma((*v1)[0], display_scale_x, display_scale_x));
        x2 = static_cast<uint8_t>(fma((*v2)[0], display_scale_x, display_scale_x));
        y1 = static_cast<uint8_t>(fma((*v1)[1], display_scale_y, display_scale_y));
        y2 = static_cast<uint8_t>(fma((*v2)[1], display_scale_y, display_scale_y));
        fb.line(x1, y1, x2, y2, true);
    }
}

void setup(void)
{
    Serial.begin(115200);
    fb.init();
    fb.clear();
    fb.flush();
    initCube();
    initModelView();
    static const float near = 1.0f;
    static const float far = 10.0f;
    static const float right = 2.0f/1.6f;
    static const float left = -2.0f/1.6f;
    static const float top = 1.0f/1.6f;
    static const float bottom = -1.0f/1.6f;
    A3D::Frustum(ProjectionMatrix, left, right, bottom, top, near, far);
    initRotationStep();
    fb.line(63, 20, 62, 19, true);
}
//~ #define PROFILING
void loop(void)
{
    #ifdef PROFILING
    long us;
    #endif
    fb.clear();
    #ifdef PROFILING
    Serial.println("Calculate new ModelView");
    us = micros();
    #endif
    animateCube();
    #ifdef PROFILING
    us = micros() - us;
    Serial.print("done, ");
    Serial.print(us);
    Serial.println(" microseconds taken");
    Serial.println("Model to world:");
    ModelWorldMatrix.print(Serial);
    Serial.println("World to view:");
    WorldViewMatrix.print(Serial);
    Serial.println("Draw cube");
    us = micros();
    #endif
    for (uint8_t i = 0; i < num_cubes; ++i)
    {
        drawCube(i);
    }
    #ifdef PROFILING
    us = micros() - us;
    Serial.print("done, ");
    Serial.print(us);
    Serial.println(" microseconds taken");
    Serial.println("Flushing framebuffer");
    us = micros();
    #endif
    fb.flush();
    #ifdef PROFILING
    us = micros() - us;
    Serial.print("done, ");
    Serial.print(us);
    Serial.println(" microseconds taken");
    #endif
}
