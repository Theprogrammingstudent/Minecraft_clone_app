#include <glad/glad.h>           // glad must always be first
#include <GLFW/glfw3.h>          // glfw second
#include <glm/glm.hpp>              // needed for glm::vec3 and glm::mat4
#include <glm/gtc/type_ptr.hpp>     // needed for glm::value_ptr
#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp> // needed for glm::rotate, glm::translate, glm::scale
#include "Chunk.h"
#include "ChunkMesher.h"

// Shaders
const char* vertexShaderSource = R"(
#version 330 core

// vertex position from the VBO
layout (location = 0) in vec3 aPos;

// model = where this object is in the world
// view  = where the camera is and what it sees
// proj  = perspective — makes far things look small
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    // order matters — projection * view * model * position
    // read right to left: move object, then apply camera, then apply perspective
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
// the final pixel color output
out vec4 FragColor;
// color sent from the CPU — vec3 is r, g, b (0.0 to 1.0)
uniform vec3 uColor;
void main()
{
    // use the color from the CPU, with 1.0 as full opacity
    FragColor = vec4(uColor, 1.0);
}
)";

// global pointer so the callback can reach the camera
// callbacks cant be member functions so we use a global
Camera* g_camera = nullptr;
float g_lastMouseX = 400.0f;
float g_lastMouseY = 300.0f;
bool g_firstMouse = true;

// GLFW calls this automatically whenever the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_firstMouse)
    {
        // on the first frame, set last position to current
        // otherwise the camera would snap wildly on startup
        g_lastMouseX = (float)xpos;
        g_lastMouseY = (float)ypos;
        g_firstMouse = false;
    }

    // how far the mouse moved since last frame
    float dx = ((float)xpos - g_lastMouseX);
    float dy = -((float)ypos - g_lastMouseY); // inverted — screen y goes down, world y goes up

    g_lastMouseX = (float)xpos;
    g_lastMouseY = (float)ypos;

    if (g_camera)
        g_camera->processMouseMovement(dx, dy);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Clean Triangle", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // without this faces draw in random order — closer faces can appear behind further ones
    glEnable(GL_DEPTH_TEST);

    // skip faces pointing away from the camera — you never see the inside of a block
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); // counter-clockwise winding = front face


    Shader shader(vertexShaderSource, fragmentShaderSource);
    Renderer renderer;

    // generate the chunk data
    Chunk chunk;

    // build the mesh from the chunk
    ChunkMesher mesher;
    mesher.buildMesh(chunk);

    // upload the mesh to the GPU — only needed once for a static chunk
    renderer.uploadMesh(mesher.vertices, mesher.indices);

    // create camera starting 3 units back from the origin, looking forward
    //Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // start further back, higher up, looking down at the chunk
    Camera camera(glm::vec3(8.0f, 10.0f, 25.0f));

    // point the global at our camera and register the callback with GLFW
    g_camera = &camera;
    glfwSetCursorPosCallback(window, mouse_callback);

    // track mouse position from the previous frame so we can calculate delta
    float lastMouseX = 400.0f; // start at screen centre (800/2)
    float lastMouseY = 300.0f; // start at screen centre (600/2)
    bool firstMouse = true;    // prevents a jump on the first frame

    // track time between frames for consistent movement speed
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // hide and lock the cursor to the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    while (!glfwWindowShouldClose(window))
    {
        // --- timing ---
        // calculate how long the last frame took
        // used to keep movement speed consistent at any frame rate
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- input ---
        camera.processKeyboard(window, deltaTime);

        // press F to toggle wireframe — lets you see the mesh structure
        static bool wireframe = false;
        static bool fWasPressed = false;
        bool fIsPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        if (fIsPressed && !fWasPressed)
        {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        fWasPressed = fIsPressed;

        // --- matrices ---
        // identity matrix — chunk sits at the world origin
        glm::mat4 model = glm::mat4(1.0f);

        // view matrix — where the camera is and what it sees
        glm::mat4 view = camera.getViewMatrix();

        // projection matrix — adds perspective
        glm::mat4 projection = camera.getProjectionMatrix(800.0f / 600.0f);

        // --- clear ---
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // clear both buffers — color is what you see, depth tracks which face is closest
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- draw chunk ---
        shader.use();
        shader.setMat4("uModel", model);
        shader.setMat4("uView", view);
        shader.setMat4("uProjection", projection);

        // solid green so we can clearly see the block faces
        shader.setVec3("uColor", glm::vec3(0.2f, 0.8f, 0.2f));

        // draw the chunk mesh
        renderer.draw((int)mesher.indices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}