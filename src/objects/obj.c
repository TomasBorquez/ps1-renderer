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
      texture.id = ShaderCreateTexture(fullPath);

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

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.length * sizeof(Vertex), VecAtPtr(mesh.vertices, 0), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.length * sizeof(u32), VecAtPtr(mesh.indices, 0), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

  // Unbind
  glBindVertexArray(0);
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
Model LoadModel(char *path, char *directory) {
  // const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
  if (scene == NULL) {
    const char *error = aiGetErrorString();
    LogError("LoadModel: failed, scene should never be null, path %s and directory %s\n Assimp Error: %s\n", path, directory, error);
    abort();
  }

  Model result = {0};
  result.directory = directory;
  result.scene = scene;
  processNode(scene->mRootNode, &result);
  return result;
}

void MeshDraw(Mesh *mesh, Object *obj) {
  u32 diffuseCount = 1;
  u32 specularCount = 1;
  for (size_t i = 0; i < mesh->textures.length; i++) {
    char name[42] = "";
    glActiveTexture(GL_TEXTURE0 + i);

    i32 number = 0;
    char *type = (VecAt(mesh->textures, i)).type;

    if (strcmp(type, "texture_diffuse")) {
      number = diffuseCount++;
    } else if (strcmp(type, "texture_specular")) {
      number = specularCount++;
    } else {
      assert(0 && "what?");
    }
    snprintf(name, 42, "material.%s%d", type, number);

    ShaderSetI(obj, name, i);
    glBindTexture(GL_TEXTURE_2D, (VecAt(mesh->textures, i)).id);
  }

  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->indices.length, GL_UNSIGNED_INT, 0);

  // Cleanup
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}
