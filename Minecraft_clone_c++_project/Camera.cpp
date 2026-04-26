#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    // position of the camera in the world
    glm::vec3 position;

    // the direction the camera is looking
    glm::vec3 front;

    // "up" direction — keeps the camera from rolling sideways
    glm::vec3 up;

    // yaw = left/right rotation, pitch = up/down rotation
    // starting values point the camera forward into the screen
    float yaw = -90.0f;
    float pitch = 0.0f;

    // how fast the camera moves and how sensitive the mouse is
    float moveSpeed = 2.5f;
    float mouseSensitivity = 0.1f;

    Camera(glm::vec3 startPosition)
        : position(startPosition),
        front(glm::vec3(0.0f, 0.0f, -1.0f)), // looking into the screen
        up(glm::vec3(0.0f, 1.0f, 0.0f))       // y axis is up
    {
    }

    // returns the view matrix — tells OpenGL where the camera is and what it sees
    // think of it as the "eyes" transform
    glm::mat4 getViewMatrix()
    {
        // lookAt takes: where we are, what we're looking at, which way is up
        return glm::lookAt(position, position + front, up);
    }

    // returns the projection matrix — adds perspective (far = small, near = big)
    // fov = field of view, aspectRatio = width/height, near/far = clipping planes
    glm::mat4 getProjectionMatrix(float aspectRatio)
    {
        return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    }

    // called every frame with WASD input
    // deltaTime keeps movement speed consistent regardless of frame rate
    void processKeyboard(GLFWwindow* window, float deltaTime)
    {
        float velocity = moveSpeed * deltaTime;

        // forward and back — move along the front vector
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * velocity;

        // left and right — strafe perpendicular to front
        // cross product of front and up gives the right vector
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(front, up)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(front, up)) * velocity;

        // up and down — move along world up axis
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            position += up * velocity;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            position -= up * velocity;

        // escape to close
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // called whenever the mouse moves
    // dx/dy = how many pixels the mouse moved since last frame
    void processMouseMovement(float dx, float dy)
    {
        // scale movement down by sensitivity
        dx *= mouseSensitivity;
        dy *= mouseSensitivity;

        yaw += dx;  // left/right rotation
        pitch += dy;  // up/down rotation

        // clamp pitch so the camera cant flip upside down past 90 degrees
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // convert yaw and pitch angles into a direction vector
        // this is standard spherical to cartesian coordinate conversion
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        // normalize so the vector length is always 1
        // without this diagonal movement would be faster than straight movement
        front = glm::normalize(direction);
    }
};