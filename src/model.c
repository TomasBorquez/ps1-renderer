#include "model.h"

Model InitModel(String path) {
  File stats;
  errno_t err = FileStats(path, &stats);

  Model result = {0};
  Arena *arenaFile = ArenaCreate((stats.size * 2.5) + 1); // NULL Terminator

  String objData;
  err = FileRead(arenaFile, path, &objData);
  if (err != SUCCESS) {
    LogError("FileRead failed, %d", err);
    abort();
  }

  Arena *arenaLines = ArenaCreate(stats.size * 6);
  StringVector lines = StrSplit(arenaLines, objData, S("\n"));
  for (size_t i = 0; i < lines.length; i++) {
    String currLine = VecAt(lines, i);

    if (currLine.data[0] == 'v' && currLine.data[1] == 't') {
      StringVector verticesSplit = StrSplit(arenaLines, currLine, S(" "));

      VecF2 vert = {0};
      // TODO: Instead of using 1/2/3 index to skip, find the first char with data and read from that
      vert.x = strtod(verticesSplit.data[2].data, NULL);
      vert.y = strtod(verticesSplit.data[3].data, NULL);

      VecPush(result.vertexTextures, vert);
      VecFree(verticesSplit);
      continue;
    }

    if (currLine.data[0] == 'v' && currLine.data[1] == 'n') {
      StringVector vertexNormalsSplit = StrSplit(arenaLines, currLine, S(" "));

      VecF3 vertNormal = {0};
      // TODO: Instead of using 1/2/3 index to skip, find the first char with data and read from that
      vertNormal.x = strtod(vertexNormalsSplit.data[2].data, NULL);
      vertNormal.y = strtod(vertexNormalsSplit.data[3].data, NULL);
      vertNormal.z = strtod(vertexNormalsSplit.data[4].data, NULL);

      VecPush(result.vertexNormals, vertNormal);
      VecFree(vertexNormalsSplit);
      continue;
    }

    if (currLine.data[0] == 'v') {
      StringVector verticesSplit = StrSplit(arenaLines, currLine, S(" "));

      VecF3 vert = {0};
      vert.x = strtod(verticesSplit.data[1].data, NULL);
      vert.y = strtod(verticesSplit.data[2].data, NULL);
      vert.z = strtod(verticesSplit.data[3].data, NULL);

      VecPush(result.vertices, vert);
      VecFree(verticesSplit);
      continue;
    }

    if (currLine.data[0] == 'f') {
      StringVector facesStringSplit = StrSplit(arenaLines, currLine, S(" "));
      for (size_t j = 1; j < facesStringSplit.length; j++) {
        StringVector facesSplit = StrSplit(arenaLines, facesStringSplit.data[j], S("/"));

        VecI3 face = {0};
        face.x = strtol(facesSplit.data[0].data, NULL, 10) - 1; // NOTE: Obj indices start from one
        face.y = strtol(facesSplit.data[1].data, NULL, 10) - 1;
        face.z = strtol(facesSplit.data[2].data, NULL, 10) - 1;

        VecPush(result.faces, face);
        VecFree(facesSplit);
      }

      VecFree(facesStringSplit);
      continue;
    }
  }

  VecFree(lines);
  ArenaFree(arenaFile);
  ArenaFree(arenaLines);
  return result;
}

VecF3 GetVertModel(Model *model, size_t faceIndex, size_t vertIndex) {
  size_t face = model->faces.data[(faceIndex * 3) + vertIndex].x;
  return model->vertices.data[face];
}

VecF2 GetVertTextModel(Model *model, size_t faceIndex, size_t vertTextIndex) {
  size_t face = model->faces.data[(faceIndex * 3) + vertTextIndex].y;
  return model->vertexTextures.data[face];
}

VecF3 GetVertNormModel(Model *model, size_t faceIndex, size_t vertNormIndex) {
  size_t face = model->faces.data[(faceIndex * 3) + vertNormIndex].z;
  return model->vertexNormals.data[face];
}
