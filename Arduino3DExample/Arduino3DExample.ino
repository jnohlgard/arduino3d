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

void printMatrix4f(const Array::Matrix4f& m)
{
    for (uint8_t y = 0; y < 4; ++y)
    {
        for (uint8_t x = 0; x < 4; ++x)
        {
            Serial.print(m(y, x));
            Serial.print('\t');
        }
        Serial.println();
    }
}

void printVector4f(const Array::Vector4f& v)
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        Serial.println(v[i]);
    }
}

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

void initProjection(void)
{
    static const float near = 1.0f;
    static const float far = 10.0f;
    static const float right = 2.0f/1.6f;
    static const float left = -2.0f/1.6f;
    static const float top = 1.0f/1.6f;
    static const float bottom = -1.0f/1.6f;
    uint8_t i = 0;
    // First row
    ProjectionMatrix[i++] = 2.0f*near/(right-left);
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = (right+left)/(right-left);
    ProjectionMatrix[i++] = 0.0f;
    // second row
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = 2.0f*near/(top-bottom);
    ProjectionMatrix[i++] = (top+bottom)/(top-bottom);
    ProjectionMatrix[i++] = 0.0f;
    // third row
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = -(far + near)/(far - near);
    ProjectionMatrix[i++] = -2*far*near/(far - near);
    // fourth row
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = 0.0f;
    ProjectionMatrix[i++] = -1.0f;
    ProjectionMatrix[i++] = 0.0f;
}

void initModelView(void)
{
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i % 5 == 0)
        {
            // Main diagonal
            ModelWorldMatrix[i] = 1.0f;
            WorldViewMatrix[i] = 1.0f;
            ScaleMatrix[i] = 1.0f;
            MoveMatrix[i] = 1.0f;
        }
        else
        {
            ModelWorldMatrix[i] = 0.0f;
            WorldViewMatrix[i] = 0.0f;
            ScaleMatrix[i] = 0.0f;
            MoveMatrix[i] = 0.0f;
        }
    }

    WorldViewMatrix[11] = -6.0f; // move cube away from camera
    MoveMatrix[11] = -3.0f; // move cube away from camera
}

void initRotationStep(void)
{
    static const float xstep = 1.0f/17.0f;
    static const float ystep = 1.0f/61.0f;
    static const float zstep = 1.0f/37.0f;
    static const float rotoffset = TWO_PI/static_cast<float>(num_cubes);
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i % 5 == 0)
        {
            // Main diagonal
            RotationXMatrix[i] = 1.0f;
            RotationYMatrix[i] = 1.0f;
            RotationZMatrix[i] = 1.0f;
            RotationOffsetMatrix[i] = 1.0f;
        }
        else
        {
            RotationXMatrix[i] = 0.0f;
            RotationYMatrix[i] = 0.0f;
            RotationZMatrix[i] = 0.0f;
            RotationOffsetMatrix[i] = 0.0f;
        }
    }
    // Rotate around X
    RotationXMatrix[5] = cos(xstep);
    RotationXMatrix[6] = -sin(xstep);
    RotationXMatrix[9] = sin(xstep);
    RotationXMatrix[10] = cos(xstep);
    // Rotate around Y
    RotationYMatrix[0] = cos(ystep);
    RotationYMatrix[2] = sin(ystep);
    RotationYMatrix[8] = -sin(ystep);
    RotationYMatrix[10] = cos(ystep);
    // Rotate around Z
    RotationZMatrix[0] = cos(zstep);
    RotationZMatrix[1] = -sin(zstep);
    RotationZMatrix[4] = sin(zstep);
    RotationZMatrix[5] = cos(zstep);

    // Rotate around Y
    RotationOffsetMatrix[0] = cos(rotoffset);
    RotationOffsetMatrix[2] = sin(rotoffset);
    RotationOffsetMatrix[8] = -sin(rotoffset);
    RotationOffsetMatrix[10] = cos(rotoffset);


    RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
}

void animateCube(void)
{
    ModelWorldMatrix *= RotationMatrix;
    ScaleMatrix[0] = ScaleMatrix[5] = ScaleMatrix[10] = (2.0 + sin(millis()/1000.0f)) * 0.3;

    MoveMatrix = RotationYMatrix * MoveMatrix;

    //~ ScaleMatrix[0] = (1.1 + sin(millis()/1000.0f))/1.5f;
    //~ ScaleMatrix[5] = (1.1 + sin(millis()/2500.0f))/1.5f;
    //~ ScaleMatrix[10] = (1.1 + sin(millis()/3500.0f))/1.5f;
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
    initProjection();
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
    printMatrix4f(ModelWorldMatrix);
    Serial.println("World to view:");
    printMatrix4f(WorldViewMatrix);
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


// Only crap below ---------------v

void testMatrix(void)
{
    long us;
    for (uint8_t i = 0; i < 4; ++i)
    {
        identity[i*5] = 1.0f;
    }

    Serial.println("Some matrices:");
    Serial.println("Identity:");
    printMatrix4f(identity);

    Serial.println("Identity x Identity:");
    Matrix4f m2(identity);
    m2 *= identity;
    printMatrix4f(m2);

    Matrix4f m3;
    for (uint8_t i = 0; i < 16; ++i)
    {
        m3[i] = i;
    }
    Serial.println("m3:");
    printMatrix4f(m3);

    Serial.println("m3 x I:");
    Matrix4f m4 = m3 * identity;
    printMatrix4f(m4);

    Serial.println("m3 x m3:");
    m4 *= m3;
    printMatrix4f(m4);

    Serial.println("m5:");
    Matrix4f m5;
    for (uint8_t i = 0; i < 16; ++i)
    {
        m5[i] = i%5;
    }
    printMatrix4f(m5);

    Serial.println("m3 x m5:");
    m4 = m3 * m5;
    printMatrix4f(m4);


    Serial.println("m3:");
    printMatrix4f(m3);
    Serial.println("m5:");
    printMatrix4f(m5);

    Serial.println("Some vectors:");
    Vector4f vx;
    Vector4f vy;
    Vector4f vz;

    vx[0] = 1.0f;
    vy[1] = 1.0f;
    vz[2] = 1.0f;

    Vector4f v1;
    for (uint8_t i = 0; i < 4; ++i)
    {
        v1[i] = i;
    }
    Serial.println("vx:");
    printVector4f(vx);
    Serial.println("vy:");
    printVector4f(vy);
    Serial.println("vz:");
    printVector4f(vz);
    Serial.println("v1:");
    printVector4f(v1);
    Serial.println("m3 * vx:");
    printVector4f(m3 * vx);
    Serial.println("m3 * vy:");
    printVector4f(m3 * vy);
    Serial.println("m3 * vz:");
    printVector4f(m3 * vz);
    Serial.println("m3 * v1:");
    printVector4f(m3 * v1);

    Serial.println(" Doing some matrix operations");
    us = micros();
    Vector4f v(2.0f);
    Matrix4f m(1.41f);
    Vector4f v2 = m*v;
    us = micros() - us;
    Serial.print("done, ");
    Serial.print(us);
    Serial.println(" microseconds taken");

}

void linesTest(void)
{
    long us;
    //~ delay(1000);
    for (uint8_t y = 3; y < 64; y += 5)
    {
        Serial.println("line");
        us = micros();
        fb.line(0, 0, 63, y, true);
        us = micros() - us;
        Serial.print("done, ");
        Serial.print(us);
        Serial.println(" microseconds taken");
        fb.flush();
        //~ delay(500);
    }
    fb.clear();
    for (uint8_t x = 0; x < 127; x += 5)
    {
        Serial.println("line");
        us = micros();
        fb.line(56, 0, x, 50, true);
        us = micros() - us;
        Serial.print("done, ");
        Serial.print(us);
        Serial.println(" microseconds taken");
        fb.flush();
        //~ delay(500);
    }
    uint8_t* data = &fb.data[0];
    uint8_t filler = 0xAA;
    for (uint16_t pos = 0; pos < 128*64/8; ++pos)
    {
        //~ *data = pos & 0xFF;
        *data = filler;
        filler = ~filler;
        ++data;
    }
    for (uint8_t i = 0; i < 255; ++i)
    {
        data = &fb.data[0];
        for (uint16_t pos = 0; pos < 128*64/8; ++pos)
        {
            //~ *data = pos & 0xFF;
            *data = filler;
            filler = ~filler;
            ++data;
        }
        filler = ~filler;
        fb.flush();
        delay(1000);
    }
}

