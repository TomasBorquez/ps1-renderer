#include "obj.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <GL/glew.h>

#include "gl.h"

static VectorTexture loadMaterialTextures(Model *model, struct aiMaterial *mat, enum aiTextureType type, char *typeName) {
  VectorTexture textures = {0};

  for (u32 i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
    struct aiString str;
    aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

    bool skip = false;
    for (size_t j = 0; j < model->texturesLoaded.length; j++) {
      if (strcmp(VecAt(model->texturesLoaded, j).path, str.data) == 0) {
        VecPush(textures, VecAt(model->texturesLoaded, j));
        skip = true;
        break;
      }
    }

    if (!skip) {
      Texture texture;
      char fullPath[512];
      snprintf(fullPath, sizeof(fullPath), "%s/%s", model->directory, str.data);
      texture.id = GLCreateTexture(fullPath);

      texture.type = typeName;
      texture.path = strdup(str.data);
      VecPush(textures, texture);
      VecPush(model->texturesLoaded, texture);
    }
  }
  return textures;
}

static Mesh meshCreate(VectorVertex vertices, VectorU32 indices, VectorTexture textures) {
  Mesh mesh = {0};
  mesh.vertices = vertices;
  mesh.indices = indices;
  mesh.textures = textures;

  GL(glGenVertexArrays(1, &mesh.VAO));
  GL(glGenBuffers(1, &mesh.VBO));
  GL(glGenBuffers(1, &mesh.EBO));

  GLBindVAO(mesh.VAO);
  GLBindVBO(mesh.VBO);
  GL(glBufferData(GL_ARRAY_BUFFER, mesh.vertices.length * sizeof(Vertex), vertices.data, GL_STATIC_DRAW));

  GLBindEBO(mesh.EBO);
  GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.length * sizeof(u32), indices.data, GL_STATIC_DRAW));

  GL(glEnableVertexAttribArray(0));
  GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position)));

  GL(glEnableVertexAttribArray(1));
  GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal)));

  GL(glEnableVertexAttribArray(2));
  GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords)));

  // Unbind
  GLUnbindVBO();
  GLUnbindVAO();
  GLUnbindEBO();
  return mesh;
}

static Mesh processMesh(Model *model, struct aiMesh *mesh) {
  VectorVertex vertices = {0};
  VectorU32 indices = {0};
  VectorTexture textures = {0};

  // Process vertices
  for (u32 i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    // Position
    vertex.position[0] = mesh->mVertices[i].x;
    vertex.position[1] = mesh->mVertices[i].y;
    vertex.position[2] = mesh->mVertices[i].z;

    // Normals
    vertex.normal[0] = mesh->mNormals[i].x;
    vertex.normal[1] = mesh->mNormals[i].y;
    vertex.normal[2] = mesh->mNormals[i].z;

    if (mesh->mTextureCoords[0]) {
      vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
      vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.texCoords[0] = 0;
      vertex.texCoords[1] = 0;
    }

    VecPush(vertices, vertex);
  }

  // Process indices
  for (u32 i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];
    for (u32 j = 0; j < face.mNumIndices; j++) {
      VecPush(indices, face.mIndices[j]);
    }
  }

  // Process material
  if (mesh->mMaterialIndex >= 0) {
    struct aiMaterial *material = model->scene->mMaterials[mesh->mMaterialIndex];
    VectorTexture diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    for (size_t i = 0; i < diffuseMaps.length; i++) {
      VecPush(textures, VecAt(diffuseMaps, i));
    }

    VectorTexture specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    for (size_t i = 0; i < specularMaps.length; i++) {
      VecPush(textures, VecAt(specularMaps, i));
    }
  }

  return meshCreate(vertices, indices, textures);
}

static void processNode(struct aiNode *node, Model *model) {
  for (u32 i = 0; i < node->mNumMeshes; i++) {
    struct aiMesh *mesh = model->scene->mMeshes[node->mMeshes[i]];
    Mesh tempMesh = processMesh(model, mesh);
    VecPush(model->meshes, tempMesh);
  }

  for (u32 i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], model);
  }
}

// WARNING: Sometimes UV flipping is not necessary
#define MAX_DIRECTORY_LENGTH 100
Model LoadModel(String path) {
  const struct aiScene *scene = aiImportFile(path.data, aiProcess_Triangulate | aiProcess_CalcTangentSpace /* | aiProcess_FlipUVs */);
  Assert(scene != NULL, "LoadModel: failed, scene should never be null, path %s\n Assimp Error: %s\n", path.data, aiGetErrorString());

  size_t lastIndex;
  for (size_t i = 0; i < path.length; i++) {
    if (path.data[i] == '/') {
      lastIndex = i;
    }
  }

  Assert(path.length < MAX_DIRECTORY_LENGTH, "LoadModel: failed, directory should not be > than directory limit, %d", 100);
  char directory[MAX_DIRECTORY_LENGTH];
  size_t directoryLength = 0;
  for (size_t i = 0; i < lastIndex; i++) {
    directory[directoryLength++] = path.data[i];
  }
  directory[directoryLength] = '\0';

  LogInfo("path: %s", path.data);
  LogInfo("directory: %s", directory);

  Model result = {0};
  result.directory = directory;
  result.scene = scene;
  processNode(scene->mRootNode, &result);
  return result;
}

#define MESH_NAME_LENGTH 42
void MeshDraw(Mesh *mesh, Object *obj) {
  u32 diffuseCount = 0;
  u32 specularCount = 0;
  for (size_t i = 0; i < mesh->textures.length; i++) {
    char name[MESH_NAME_LENGTH] = "";
    GL(glActiveTexture(GL_TEXTURE0 + i));

    i32 number = 0;
    char *type = VecAt(mesh->textures, i).type;
    if (strcmp(type, "texture_diffuse") == 0) {
      number = ++diffuseCount;
    } else if (strcmp(type, "texture_specular") == 0) {
      number = ++specularCount;
    } else {
      Assert(0, "MeshDraw: Failed, type %s does not exist", type);
    }
    snprintf(name, MESH_NAME_LENGTH, "material.%s%d", type, number);

    GLSetUniformI(obj, name, i);
    GL(glBindTexture(GL_TEXTURE_2D, (VecAt(mesh->textures, i)).id));
  }

  GLBindVAO(mesh->VAO);
  GL(glDrawElements(GL_TRIANGLES, mesh->indices.length, GL_UNSIGNED_INT, 0));

  // Cleanup
  GLUnbindVAO();
  GL(glActiveTexture(GL_TEXTURE0));
}

void ObjModelMatReset(Object *obj) {
  glm_mat4_copy((mat4)GLM_MAT4_IDENTITY_INIT, obj->modelMat);
}

void ObjRotate(Object *obj, f32 angle, Axis axis) {
  vec3 axisVec;
  switch (axis) {
  case x:
    axisVec[0] = 1;
    break;
  case y:
    axisVec[1] = 1;
    break;
  case z:
    axisVec[2] = 1;
    break;
  }
  glm_rotate(obj->modelMat, angle, axisVec);
}

void ObjScale(Object *obj, f32 x, f32 y, f32 z) {
  glm_scale(obj->modelMat, (vec3){x, y, z});
}

void ObjTranslate(Object *obj, f32 x, f32 y, f32 z) {
  glm_translate(obj->modelMat, (vec3){x, y, z});
}
