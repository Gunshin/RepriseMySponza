#pragma once

#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "ShaderProgram.hpp"

class MyScene;

class MyView : public tygra::WindowViewDelegate
{
public:
	
    MyView();
	
    ~MyView();

    void
    setScene(std::shared_ptr<const MyScene> scene);

private:

    void
    windowViewWillStart(std::shared_ptr<tygra::Window> window);
	
    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height);

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window);
    
    void
    windowViewRender(std::shared_ptr<tygra::Window> window);

    std::shared_ptr<const MyScene> scene_;

    void UpdateLightData();

    float aspectRatio;

    struct Vertex
    {
        Vertex(){};
        Vertex(glm::vec3 pos_, glm::vec3 norm_/*, glm::vec2 tex_*/) : position(pos_), normal(norm_)/*, texCoord(tex_)*/ {}
        glm::vec3 position, normal;
        //glm::vec2 texCoord;
    };

    GLuint vertexVBO; // VertexBufferObject for the vertex positions
    GLuint elementVBO; // VertexBufferObject for the elements (indices)

    struct Mesh
    {
        GLuint vao;// VertexArrayObject for the shape's vertex array settings
        int startVerticeIndex, endVerticeIndex, verticeCount;
        int startElementIndex, endElementIndex, element_count; // Needed for when we draw using the vertex arrays

        Mesh() : startVerticeIndex(0),
            endVerticeIndex(0),
            verticeCount(0),
            startElementIndex(0),
            endElementIndex(0),
            element_count(0) {}
    };
    std::vector< Mesh > loadedMeshes;

    struct MaterialData
    {
        glm::vec3 colour;
        float shininess;
    };
    std::vector< MaterialData > materials;
    GLuint bufferMaterials;

    struct InstanceData
    {
        glm::mat4x3 positionData;
        GLint materialDataIndex;
    };
    std::vector< GLuint > instanceVBOs;
    std::vector< std::vector< InstanceData > > instanceData;

    // cant get access to the MyScene::Light since we are only declaring MyScene as a class (no direct reference)
    struct LightData
    {
        glm::vec3 position;
        float range;
        glm::vec3 direction;
        float field_of_view_degrees;
    };
    std::vector<LightData> lights;
    GLuint bufferLights;

    ShaderProgram shaderProgram;

};
