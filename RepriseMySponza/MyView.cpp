#include "MyView.hpp"
#include "MyScene.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>

#include "Shader.hpp"
#include "ShaderProgram.hpp"

MyView::
MyView()
{
}

MyView::
~MyView() {
}

void MyView::
setScene(std::shared_ptr<const MyScene> scene)
{
    scene_ = scene;
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);

    Shader vs, fs;
    vs.loadShader("reprise_vs.glsl", GL_VERTEX_SHADER);
    fs.loadShader("reprise_fs.glsl", GL_FRAGMENT_SHADER);

    shaderProgram.createProgram();
    shaderProgram.addShaderToProgram(&vs);
    shaderProgram.addShaderToProgram(&fs);
    shaderProgram.linkProgram();

    shaderProgram.useProgram();
    
    instanceData.resize(scene_->meshCount());
    instanceVBOs.resize(scene_->meshCount());

    for (int i = 0; i < scene_->modelCount(); ++i)
    {
        InstanceData instance;
        instance.positionData = scene_->model(i).xform;
        instance.materialDataIndex = static_cast< GLint >(scene_->model(i).material_index);
        instanceData[scene_->model(i).mesh_index].push_back(instance);
    }

    

    /******
    Since im not someone who likes to assume things, i have attempted to jump around the problem that each individual model
    (even those with the same meshes) could potentially have different materials as is specified in the model data. I have
    done this by using 2d texture arrays. What i am doing is creating the 2d texture array, and loading the images into it
    in the same order that the scene gives me the materials, and then combining the colour(vec3) and shininess(float) into
    a single vec4 with the alpha component being shininess.
    ******/

    for (int i = 0; i < scene_->materialCount(); ++i)
    {
        MaterialData data;
        data.colour = scene_->material(i).colour;
        data.shininess = scene_->material(i).shininess;
        materials.push_back(data);
    }

    // setup material SSBO
    glGenBuffers(1, &bufferMaterials);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferMaterials);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialData) * materials.size(), &materials[0], GL_STREAM_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufferMaterials);
    glShaderStorageBlockBinding(
        shaderProgram.getProgramID(),
        glGetUniformBlockIndex(shaderProgram.getProgramID(), "BufferMaterials"),
        0);

    //copy lights data
    UpdateLightData();

    // set up light SSBO
    glGenBuffers(1, &bufferRender);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferRender);
    unsigned int size = sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(LightData) + sizeof(float); // since our buffer only has a projection matrix, camposition and single light source
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferRender);
    glShaderStorageBlockBinding(
        shaderProgram.getProgramID(),
        glGetUniformBlockIndex(shaderProgram.getProgramID(), "BufferRender"),
        1);

    //load scene meshes
    std::vector<Vertex> vertices;
    std::vector< unsigned int > elements;
    for (int i = 0; i < scene_->meshCount(); ++i)
    {
        Mesh mesh;
        mesh.startVerticeIndex = vertices.size();
        mesh.startElementIndex = elements.size();
        for (unsigned int j = 0; j < scene_->mesh(i).position_array.size(); ++j)
        {
            vertices.push_back(Vertex(scene_->mesh(i).position_array[j],
                scene_->mesh(i).normal_array[j]/*,
                scene_->mesh(i).texcoord_array[j]*/));
        }
        for (unsigned int j = 0; j < scene_->mesh(i).element_array.size(); ++j)
        {
            elements.push_back(scene_->mesh(i).element_array[j]);
        }

        mesh.endVerticeIndex = vertices.size() - 1;
        mesh.endElementIndex = elements.size() - 1;
        mesh.verticeCount = mesh.endVerticeIndex - mesh.startVerticeIndex;
        mesh.element_count = mesh.endElementIndex - mesh.startElementIndex + 1;
        loadedMeshes.push_back(mesh);
    }

    // set up vao
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        elements.size() * sizeof(unsigned int),
        elements.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i = 0; i < scene_->meshCount(); ++i)
    {
        glGenBuffers(1, &instanceVBOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER,
            instanceData[i].size() * sizeof(InstanceData),
            instanceData[i].data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        unsigned int offset = 0;

        glGenVertexArrays(1, &loadedMeshes[i].vao);
        glBindVertexArray(loadedMeshes[i].vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), TGL_BUFFER_OFFSET(offset));
        offset += sizeof(glm::vec3);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), TGL_BUFFER_OFFSET(offset));
        offset += sizeof(glm::vec3);

        /*glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), TGL_BUFFER_OFFSET(offset));
        offset += sizeof(glm::vec2);*/

        unsigned int instanceOffset = 0;
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);

        for (int a = 2; a < 6; ++a)
        {
            glEnableVertexAttribArray(a);
            glVertexAttribPointer(a, 3, GL_FLOAT, GL_FALSE,
                sizeof(InstanceData), TGL_BUFFER_OFFSET(instanceOffset));
            glVertexAttribDivisor(a, 1);
            instanceOffset += sizeof(glm::vec3);
        }

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE,
            sizeof(InstanceData), TGL_BUFFER_OFFSET(instanceOffset));
        glVertexAttribDivisor(6, 1);
        instanceOffset += sizeof(GLint);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
                   int width,
                   int height)
{
    glViewport(0, 0, width, height);
    aspectRatio = static_cast<float>(width) / height;
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.25f, 0.f);

    glm::mat4 projectionMatrix = glm::perspective(75.f, aspectRatio, 1.f, 1000.f);
    glm::mat4 viewMatrix = glm::lookAt(scene_->camera().position, scene_->camera().direction + scene_->camera().position, glm::vec3(0, 1, 0));
    glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;

    // make sure all light data is up to date.
    UpdateLightData();

	// draw depth buffer
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_BLEND);
    
    SetBuffer(projectionViewMatrix, scene_->camera().position, lights[0]);
    for (int i = 0; i < scene_->meshCount(); ++i)
    {
        glBindVertexArray(loadedMeshes[i].vao);
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
            loadedMeshes[i].element_count,
            GL_UNSIGNED_INT,
            TGL_BUFFER_OFFSET(loadedMeshes[i].startElementIndex * sizeof(int)),
            instanceData[i].size(),
            loadedMeshes[i].startVerticeIndex);
    }

    // draw colour
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_EQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    for (int lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
    {
		SetBuffer(projectionViewMatrix, scene_->camera().position, lights[lightIndex]);

        for (int i = 0; i < scene_->meshCount(); ++i)
        {
            glBindVertexArray(loadedMeshes[i].vao);
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                loadedMeshes[i].element_count,
                GL_UNSIGNED_INT,
                TGL_BUFFER_OFFSET(loadedMeshes[i].startElementIndex * sizeof(int)),
                instanceData[i].size(),
                loadedMeshes[i].startVerticeIndex);
        }
    }
    
    glDepthMask(GL_TRUE);
}

void MyView::UpdateLightData()
{
    lights.resize(scene_->lightCount());
    for (int i = 0; i < scene_->lightCount(); ++i)
    {
        //memcpy the data since someone decided to copy the wrong file, and then give us lights from a damn switch statement. crazy i tell ya
        memcpy(&lights[i], &scene_->light(i), sizeof(LightData));
    }
}

void MyView::SetBuffer(glm::mat4 projectMat_, glm::vec3 camPos_, LightData light_)
{
    // so since glMapBufferRange does not work, i am going to create a temporary buffer for the per model data, and then copy the full buffer straight into the shaders buffer
    unsigned int bufferSize = sizeof(projectMat_)+sizeof(camPos_)+sizeof(float)/*padding0*/ +sizeof(LightData);
    char* buffer = new char[bufferSize];
    unsigned int index = 0;

    //projection matrix first!
    memcpy(buffer + index, glm::value_ptr(projectMat_), sizeof(glm::mat4));
    index += sizeof(projectMat_);

    // camera position next!
    memcpy(buffer + index, glm::value_ptr(camPos_), sizeof(camPos_));
    index += sizeof(camPos_)+sizeof(float);//padding

    // finally the LIGHTS!
    memcpy(buffer + index, &light_, sizeof(LightData));

    // update the render buffer, so get the pointer!
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferRender);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, buffer, bufferSize);
    //done
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    delete[] buffer;
}