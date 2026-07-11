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
#include "Texture.h"
#include <iostream>
#include <filesystem>
#include <fstream>


// Shaders
const char* vertexShaderSource = R"(
#version 330 core

// position attribute — location 0, matches glVertexAttribPointer(0, ...)
layout (location = 0) in vec3 aPos;

// texture coordinate attribute — location 1, matches glVertexAttribPointer(1, ...)
layout (location = 1) in vec2 aTexCoord;

// matrices sent from CPU every frame via shader.setMat4()
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// pass UV to the fragment shader
// out here = in on the fragment shader side
out vec2 vTexCoord;

void main()
{
    // pass UV straight through — no modification needed
    vTexCoord = aTexCoord;

    // standard MVP transform — place object, apply camera, apply perspective
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

// UV coordinate passed in from vertex shader
in vec2 vTexCoord;

// the texture atlas — bound in main.cpp before drawing
// sampler2D is the GLSL type for a 2D texture
uniform sampler2D uTexture;

void main()
{
    // sample the texture at this UV coordinate
    // texture() looks up the colour in the atlas at position vTexCoord
    FragColor = texture(uTexture, vTexCoord);
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
    // print the current working directory so we know where the exe is looking
    std::cout << "========== FILE CHECK ==========" << std::endl;
    std::cout << "Working directory: "
        << std::filesystem::current_path()
        << std::endl;

    // ADD THESE LINES HERE — before the texture load
    std::ifstream test("assets\\textures\\terrain_atlas.png");
    if (test.good())
        std::cout << "FILE EXISTS" << std::endl;
    else
        std::cout << "FILE NOT FOUND" << std::endl;
    test.close();


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

    // load the texture atlas from disk into GPU memory
    // path is relative to where the .exe runs — post-build event copies it there
    Texture texture("assets\\textures\\terrain_atlas.png");

    std::cout << "========== TEXTURE CHECK ==========" << std::endl;
    if (texture.ID == 0)
        std::cerr << ">>> TEXTURE FAILED TO LOAD" << std::endl;
    else
        std::cout << ">>> TEXTURE OK — ID: " << texture.ID << std::endl;
    std::cout << "====================================" << std::endl;

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

        // bind the texture to slot 0 before drawing
        // the shader samples from slot 0 by default (uTexture)
        texture.bind(0);

        // tell the shader which slot to sample from
        // 0 matches GL_TEXTURE0 which is where we bound the texture
        shader.setInt("uTexture", 0);

        // world.draw handles the model matrix and draw call for each chunk
        world.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}