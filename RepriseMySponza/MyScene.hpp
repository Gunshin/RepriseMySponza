#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <memory>

class FirstPersonMovement;

class MyScene
{
public:
    
    MyScene();

    ~MyScene();

    void
    setCameraTranslationSpeed(float sideward,
                              float forward);

    void
    setCameraRotationSpeed(float horizontal,
                           float vertical);

    void
    update();

    bool
    toggleCameraAnimation();

    float
    time() const;

    glm::vec3
    upDirection() const;

    struct Camera
    {
        glm::vec3 position;
        glm::vec3 direction;
        float vertical_field_of_view_degrees;
        float near_plane_distance;
        float far_plane_distance;
    };

    Camera
    camera() const;

    struct Light
    {
        glm::vec3 position;
        float range;
        glm::vec3 direction;
        float field_of_view_degrees;
    };

    int
    lightCount() const;

    Light
    light(int index) const;

    glm::vec3
    globalLightDirection() const;

    struct Material
    {
        glm::vec3 colour;
        float shininess;
        bool metallic;
    };

    int
    materialCount() const;

    Material
    material(int index) const;

    struct Mesh
    {
        std::vector<glm::vec3> position_array;
        std::vector<glm::vec3> normal_array;
        std::vector<glm::vec2> texcoord_array;
        std::vector<unsigned int> element_array;
        std::vector<unsigned int> instance_array;
    };

    int
    meshCount() const;

    const Mesh&
    mesh(int index) const;

    struct Model
    {
        unsigned int mesh_index;
        unsigned int material_index;
        glm::mat4x3 xform;
    };

    int
    modelCount() const;

    Model
    model(int index) const;

private:

    bool
    readFile(std::string filepath);

    std::chrono::system_clock::time_point start_time_;
    float time_seconds_;

    bool animate_camera_;

    std::shared_ptr<FirstPersonMovement> camera_;
    glm::vec3 camera_translation_speed_;
    glm::vec2 camera_rotation_speed_;

    std::vector<Mesh> meshes_;

    std::vector<Model> models_;

    std::vector<Material> materials_;
};
