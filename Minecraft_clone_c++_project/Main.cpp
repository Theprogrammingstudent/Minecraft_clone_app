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
#include "World.h"

// Shaders
const char* vertexShaderSource = R"(
#version 330 core

// vertex position from the VBO
layout (location = 0) in vec3 aPos;

// model, view, projection matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// pass the height to the fragment shader so it can colour by block type
out float vHeight;

void main()
{
    // pass the y position to the fragment shader
    // fragment shader will use this to pick a colour
    vHeight = aPos.y;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

// height value passed in from the vertex shader
in float vHeight;

void main()
{
    // colour based on height — gives each layer a distinct colour
    // these ranges match the height values from your noise generator
    if (vHeight >= 9.0)
        // grass — green
        FragColor = vec4(0.2, 0.7, 0.2, 1.0);
    else if (vHeight >= 7.0)
        // dirt — brown
        FragColor = vec4(0.5, 0.3, 0.1, 1.0);
    else
        // stone — grey
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft Clone", NULL, NULL);
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

    // world manages all chunks — generates and uploads them all on construction
    World world;

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

    // cursor is visible and free — click window to focus, mouse still controls camera
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


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

        // ADD THESE — matrices and clear were lost in the refactor

        // view matrix — where the camera is and what it sees
        glm::mat4 view = camera.getViewMatrix();

        // projection matrix — perspective, using window aspect ratio
        glm::mat4 projection = camera.getProjectionMatrix(800.0f / 600.0f);

        // clear color and depth buffer each frame
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate the shader before sending uniforms
        shader.use();

        // send view and projection once — they're the same for all chunks
        shader.setMat4("uView", view);
        shader.setMat4("uProjection", projection);

        // world.draw handles the model matrix and draw call for each chunk
        world.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}