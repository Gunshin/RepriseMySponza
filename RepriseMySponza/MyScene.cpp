#include "MyScene.hpp"
#include "FirstPersonMovement.hpp"
#include <tcf/SimpleScene.hpp>
#include <iostream>

MyScene::
MyScene()
{
    start_time_ = std::chrono::system_clock::now();
    time_seconds_ = 0.f;

    if (!readFile("sponza_with_friends_2x.tcf")) {
        std::cerr << "Failed to read sponza_with_friends.tcf data file"
                  << std::endl;
    }

    animate_camera_ = false;
    camera_.reset(new FirstPersonMovement());
    camera_->init(glm::vec3(80, 50, 0), 1.5f, 0.5f);
}

MyScene::
~MyScene()
{
}

void MyScene::
setCameraTranslationSpeed(float sideward,
                          float forward)
{
    camera_translation_speed_.x = sideward;
    camera_translation_speed_.y = 0;
    camera_translation_speed_.z = forward;
}

void MyScene::
setCameraRotationSpeed(float horizontal,
                       float vertical)
{
    camera_rotation_speed_.x = horizontal;
    camera_rotation_speed_.y = vertical;
}

bool MyScene::
readFile(std::string filepath)
{
    tcf::Error error;
    tcf::SimpleScene tcf_scene = tcf::simpleSceneFromFile(filepath, &error);
    if (error != tcf::kNoError) {
        return false;
    }

    meshes_.clear();
    models_.clear();

    meshes_.reserve(tcf_scene.meshArray.size());
    for (const auto& mesh : tcf_scene.meshArray) {
        Mesh new_mesh;
        new_mesh.instance_array.reserve(mesh.instanceArray.size());
        models_.reserve(models_.size() + mesh.instanceArray.size());
        for (const auto& model : mesh.instanceArray) {
            Model new_model;
            new_model.mesh_index = meshes_.size();
			new_model.material_index = 0;
            new_model.xform = glm::mat4x3(model.m00, model.m01, model.m02,
                                          model.m10, model.m11, model.m12,
                                          model.m20, model.m21, model.m22,
                                          model.m30, model.m31, model.m32);
            new_mesh.instance_array.push_back(models_.size());
            models_.push_back(new_model);
        }
        new_mesh.element_array.assign((unsigned int*)&mesh.indexArray.front(),
                                      (unsigned int*)&mesh.indexArray.back()+1);
        new_mesh.normal_array.assign((glm::vec3*)&mesh.normalArray.front(),
                                     (glm::vec3*)&mesh.normalArray.back()+1);
        new_mesh.position_array.assign((glm::vec3*)&mesh.vertexArray.front(),
                                       (glm::vec3*)&mesh.vertexArray.back()+1);
        if (!mesh.texcoordArray.empty()) {
            new_mesh.texcoord_array.assign(
                (glm::vec2*)&mesh.texcoordArray.front(),
                (glm::vec2*)&mesh.texcoordArray.back()+1);
        }
        meshes_.push_back(new_mesh);
    }

    // hardcode some material data ... not good practice!
    int redShapes[] = { 35, 36, 37, 38, 39, 40, 41, 42, 69, 70, 71, 72, 73, 74,
                        75, 76, 77, 78, 79 };
    int greenShapes[] = { 8, 19, 31, 33, 54, 57, 67, 68, 66, 80 };
    int yellowShapes[] = { 4, 5, 6, 7, 23, 24, 25, 26, 27, 28, 29, 30, 44, 45,
                           46, 47, 48, 49, 50, 51, 52, 53 };
    int happyShapes[] = { 82 };
    int bunnyShapes[] = { 81 };
    int dragonShapes[] = { 83 };
    int *shapes[6] = { redShapes, greenShapes, yellowShapes,
                       happyShapes, bunnyShapes, dragonShapes };
    int numberOfShapes[] = { sizeof(redShapes) / sizeof(int),
                             sizeof(greenShapes) / sizeof(int),
                             sizeof(yellowShapes) / sizeof(int),
                             sizeof(happyShapes) / sizeof(int),
                             sizeof(bunnyShapes) / sizeof(int),
                             sizeof(dragonShapes) / sizeof(int) };
    glm::vec3 colours[6] = { glm::vec3(1.f, 0.33f, 0.f),
                             glm::vec3(0.2f, 0.8f, 0.2f),
                             glm::vec3(0.8f, 0.8f, 0.2f),
                             glm::vec3(0.8f, 0.4f, 0.4f),
                             glm::vec3(0.4f, 0.8f, 0.4f),
                             glm::vec3(0.4f, 0.4f, 0.8f) };
    float shininess[6] = { 0.f, 64.f, 128.f, 64.f, 0.f, 0.f };
    bool metallic[6] = { true, true, false, true, false, false };
    Material new_material;
    new_material.colour = glm::vec3(0.8f, 0.8f, 0.8f);
    new_material.shininess = 0.f;
    new_material.metallic = true;
    materials_.push_back(new_material);
    for (int j=0; j<6; ++j) {
        Material new_material;
        new_material.colour = colours[j];
        new_material.shininess = shininess[j];
        new_material.metallic = metallic[j];
        materials_.push_back(new_material);
        for (int i=0; i<numberOfShapes[j]; ++i) {
            int index = shapes[j][i];
            models_[index].material_index = j+1;
        }
    }

    return true;
}

void MyScene::
update()
{
    const auto clock_time = std::chrono::system_clock::now() - start_time_;
    const auto clock_millisecs
        = std::chrono::duration_cast<std::chrono::milliseconds>(clock_time);
    const float prev_time = time_seconds_;
    time_seconds_ = 0.001f * clock_millisecs.count();
    const float dt = time_seconds_ - prev_time;

    if (!animate_camera_) {
        camera_->moveForward(camera_translation_speed_.z * dt);
        camera_->moveRight(camera_translation_speed_.x * dt);
        camera_->spinHorizontal(camera_rotation_speed_.x * dt);
        camera_->spinVertical(camera_rotation_speed_.y * dt);
    }
}

bool MyScene::
toggleCameraAnimation()
{
    return animate_camera_ = !animate_camera_;
}

float MyScene::
time() const
{
    return time_seconds_;
}

glm::vec3 MyScene::
upDirection() const
{
    return glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 MyScene::
globalLightDirection() const
{
    return glm::normalize(glm::vec3(-10, -5, -2));
}

MyScene::Camera MyScene::
camera() const
{
    Camera cam;
    if (animate_camera_) {
        const float t = -0.3f * time_seconds_;
        const float ct = cosf(t);
        const float rx = ct < 0 ? -120.f : 120.f;
        const float st = sinf(t);
        const float rz = st < 0 ? -20.f : 20.f;
        const float m = 0.1f;
        const float ry = st < 0 ? -30.f : 30.f;
        glm::vec3 look_at(0, 30, 0);
        cam.position = glm::vec3(rx * powf(fabs(ct), m),
                                 50 + ry * powf(fabs(st), m),
                                 25.f + rz * powf(fabs(st), m));
        cam.direction = glm::normalize(look_at - cam.position);
    } else {
        cam.position = camera_->position();
        cam.direction = camera_->direction();
    }
    cam.vertical_field_of_view_degrees = 75.f;
    cam.near_plane_distance = 1.f;
    cam.far_plane_distance = 1000.f;
    return cam;
}

int MyScene::
lightCount() const
{
    return 5;
}

MyScene::Light MyScene::
light(int index) const
{
    Light light;
    const float t = time_seconds_;

    switch (index) {
    case 0:
        light.position = glm::vec3(75.f, 110.f, -5.f + 15.f * cosf(t));
        light.range = 300;
        light.direction = glm::normalize(glm::vec3(-40.f, 0.f, -5.f)
                                         - light.position);
        light.field_of_view_degrees = 60.f;
        break;
    case 1:
        light.position = glm::vec3(-75.f, 110.f, -5.f + 15.f * cosf(1+t));
        light.range = 300;
        light.direction = glm::normalize(glm::vec3(40.f, 0.f, -5.f)
                                         - light.position);
        light.field_of_view_degrees = 60.f;
        break;
    case 2:
        light.position = glm::vec3(-97.75f, 8.34f, -5.25f + 3.f * cosf(3+t));
        light.range = 100;
        light.direction = glm::normalize(glm::vec3(0.97f, 0.26f, 0.f));
        light.field_of_view_degrees = 90.f;
        break;
    case 3:
        light.position = glm::vec3(-20.56f, 4.17f, 0.44f + 3.f * cosf(4+t));
        light.range = 100;
        light.direction = glm::normalize(glm::vec3(0.94f, 0.09f, -0.33f));
        light.field_of_view_degrees = 90.f;
        break;
    case 4:
        light.position = glm::vec3(37.88f, 3.36f, -10.53f + 3.f * cosf(3+t));
        light.range = 100;
        light.direction = glm::normalize(glm::vec3(0.84f, 0.37f, 0.42f));
        light.field_of_view_degrees = 90.f;
        break;
    }

    return light;
}

int MyScene::
materialCount() const
{
    return materials_.size();
}

MyScene::Material MyScene::
material(int index) const
{
    return materials_[index];
}

int MyScene::
meshCount() const
{
    return meshes_.size();
}

const MyScene::Mesh& MyScene::
mesh(int index) const
{
    return meshes_[index];
}

int MyScene::
modelCount() const
{
    return models_.size();
}

MyScene::Model MyScene::
model(int index) const
{
    return models_[index];
}
