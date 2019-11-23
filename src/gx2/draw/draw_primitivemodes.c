#include <gfd.h>
#include <coreinit/debug.h>
#include <coreinit/memdefaultheap.h>
#include <gx2/draw.h>
#include <gx2/registers.h>
#include <gx2/shaders.h>
#include <gx2/mem.h>
#include <gx2r/draw.h>
#include <gx2r/buffer.h>
#include <string.h>
#include <stdio.h>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/proc.h>
#include <whb/sdcard.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <stdalign.h>

#define GB_MATH_IMPLEMENTATION
#include <gb_math.h>

static float alignas(GX2_VERTEX_BUFFER_ALIGNMENT)
sPositionData[] =
{
   // 0 1
   // 4 5
   // 3 2
   -0.5f,  0.5f,  0.0f,
    0.5f,  0.5f,  0.0f,
    0.5f, -0.5f,  0.0f,
   -0.5f, -0.5f,  0.0f,
   -0.5f,  0.0f,  0.0f,
    0.5f,  0.0f,  0.0f,
};

static float alignas(GX2_VERTEX_BUFFER_ALIGNMENT)
sColourData[] =
{
    1.0f,  0.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  0.0f, 1.0f,
    0.0f,  0.0f,  1.0f, 1.0f,
    1.0f,  1.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  1.0f, 1.0f,
    1.0f,  0.0f,  1.0f, 1.0f,
};

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesPoints[] = { 0, 1, 2, 3 };


static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesLines[] = { 0, 1, 1, 2, 2, 3, 3, 0 };

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesLineStrip[] = { 0, 1, 2, 3, 0 };

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesLineLoop[] = { 0, 1, 2, 3 };


static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesTriangleList[] = { 0, 1, 3, 1, 2, 3 };

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesTriangleFan[] = { 0, 1, 2, 3 };

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesTriangleStrip[] = { 0, 1, 3, 2 };


static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesRects[] = { 0, 1, 4, 4, 5, 3 };


static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesQuads[] = { 0, 1, 2, 3 };

static int alignas(GX2_INDEX_BUFFER_ALIGNMENT)
sIndicesQuadStrip[] = { 0, 1, 4, 5, 3, 2 };

#define countof(x) (sizeof(x) / sizeof(x[0]))

int main(int argc, char **argv)
{
   WHBGfxShaderGroup group = { 0 };
   GX2UniformVar *uniformVar = NULL;
   char *gshFileData = NULL;
   char *sdRootPath = NULL;
   char path[256];
   int result = 0;
   int uniformModelMatrixLoc = -1;
   int uniformViewMatrixLoc = -1;
   int uniformProjectionMatrixLoc = -1;

   WHBLogUdpInit();
   WHBProcInit();
   WHBGfxInit();

   if (!WHBMountSdCard()) {
      result = -1;
      goto exit;
   }

   sdRootPath = WHBGetSdCardMountPath();
   sprintf(path, "%s/wiiu-tests/content/shaders/pos_colour_mvp.gsh", sdRootPath);

   gshFileData = WHBReadWholeFile(path, NULL);
   if (!WHBGfxLoadGFDShaderGroup(&group, 0, gshFileData)) {
      result = -1;
      WHBLogPrintf("WHBGfxLoadGFDShaderGroup returned  FALSE");
      goto exit;
   }

   WHBGfxInitShaderAttribute(&group, "position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
   WHBGfxInitShaderAttribute(&group, "colour", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitFetchShader(&group);

   WHBFreeWholeFile(gshFileData);
   gshFileData = NULL;

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "modelMatrix")) {
      uniformModelMatrixLoc = uniformVar->offset;
   }

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "viewMatrix")) {
      uniformViewMatrixLoc = uniformVar->offset;
   }

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "projectionMatrix")) {
      uniformProjectionMatrixLoc = uniformVar->offset;
   }

   struct {
      GX2PrimitiveMode mode;
      int *indices;
      int numIndices;
      gbMat4 modelMatrix;
   } primitives[] = {
      { GX2_PRIMITIVE_MODE_POINTS, sIndicesPoints, countof(sIndicesPoints) },
      { GX2_PRIMITIVE_MODE_LINES, sIndicesLines, countof(sIndicesLines) },
      { GX2_PRIMITIVE_MODE_LINE_STRIP, sIndicesLineStrip, countof(sIndicesLineStrip) },
      { GX2_PRIMITIVE_MODE_LINE_LOOP, sIndicesLineLoop, countof(sIndicesLineLoop) },
      { GX2_PRIMITIVE_MODE_TRIANGLES, sIndicesTriangleList, countof(sIndicesTriangleList) },
      { GX2_PRIMITIVE_MODE_TRIANGLE_FAN, sIndicesTriangleFan, countof(sIndicesTriangleFan) },
      { GX2_PRIMITIVE_MODE_TRIANGLE_STRIP, sIndicesTriangleStrip, countof(sIndicesTriangleStrip) },
      { GX2_PRIMITIVE_MODE_RECTS, sIndicesRects, countof(sIndicesRects) },
      { GX2_PRIMITIVE_MODE_QUADS, sIndicesQuads, countof(sIndicesQuads) },
      { GX2_PRIMITIVE_MODE_QUAD_STRIP, sIndicesQuadStrip, countof(sIndicesQuadStrip) },
   };

   // 16:9
   gbMat4 projectionMatrix;
   gb_mat4_ortho3d(&projectionMatrix, -8.0f, 8.0f, -4.5f, 4.5f, 0.0f, 20.0f);

   gbMat4 viewMatrix;
   gbVec3 eye = { 0.0f, 0.0f, 10.0f };
   gbVec3 center = { 0.0f, 0.0f, 0.0f };
   gbVec3 up = { 0.0f, 1.0f, 0.0f };
   gb_mat4_look_at(&viewMatrix, eye, center, up);

   gbMat4 scaleMatrix;
   gbMat4 translateMatrix;
   gb_mat4_scalef(&scaleMatrix, 2.5f);

   float posX = -6.0f;
   float posY = 2.0f;
   for (int i = 0; i < countof(primitives); ++i) {
      gbVec3 pos = { posX, posY, 0.0f };
      gbVec3 scale = { 5.0f, 5.0f, 1.0f };
      gb_mat4_translate(&translateMatrix, pos);
      gb_mat4_mul(&primitives[i].modelMatrix, &translateMatrix, &scaleMatrix);
      posX += 16 / (countof(primitives) / 2);
      if (i == (countof(primitives) / 2) - 1) {
         posX = -6.0f;
         posY = -2.0f;
      }

      GX2Invalidate(GX2_INVALIDATE_MODE_CPU, &primitives[i].modelMatrix, sizeof(primitives[i].modelMatrix));
      GX2Invalidate(GX2_INVALIDATE_MODE_CPU, primitives[i].indices, sizeof(int) * primitives[i].numIndices);
   }

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU, &sPositionData, sizeof(sPositionData));
   GX2Invalidate(GX2_INVALIDATE_MODE_CPU, &sColourData, sizeof(sColourData));
   GX2Invalidate(GX2_INVALIDATE_MODE_CPU, &viewMatrix, sizeof(viewMatrix));
   GX2Invalidate(GX2_INVALIDATE_MODE_CPU, &projectionMatrix, sizeof(projectionMatrix));

   gbMat4 rotateMatrix;

   WHBLogPrintf("Begin rendering ...");
   while (WHBProcIsRunning()) {
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(0.2f, 0.2f, 0.2f, 1.0f);
      GX2SetPointSize(8.0f, 8.0f);
      GX2SetLineWidth(4.0f);
      GX2SetVertexUniformReg(uniformViewMatrixLoc, 16, &viewMatrix);
      GX2SetVertexUniformReg(uniformProjectionMatrixLoc, 16, &projectionMatrix);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2SetAttribBuffer(0, sizeof(sPositionData), 4 * 3, sPositionData);
      GX2SetAttribBuffer(1, sizeof(sColourData), 4 * 4, sColourData);
      for (int i = 0; i < countof(primitives); ++i) {
         GX2SetVertexUniformReg(uniformModelMatrixLoc, 16, &primitives[i].modelMatrix);
         GX2DrawIndexedEx(primitives[i].mode, primitives[i].numIndices, GX2_INDEX_TYPE_U32, primitives[i].indices, 0, 1);
      }
      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(0.2f, 0.2f, 0.2f, 1.0f);
      GX2SetPointSize(4.0f, 4.0f);
      GX2SetLineWidth(4.0f);
      GX2SetVertexUniformReg(uniformViewMatrixLoc, 16, &viewMatrix);
      GX2SetVertexUniformReg(uniformProjectionMatrixLoc, 16, &projectionMatrix);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2SetAttribBuffer(0, sizeof(sPositionData), 4 * 3, sPositionData);
      GX2SetAttribBuffer(1, sizeof(sColourData), 4 * 4, sColourData);
      for (int i = 0; i < countof(primitives); ++i) {
         GX2SetVertexUniformReg(uniformModelMatrixLoc, 16, &primitives[i].modelMatrix);
         GX2DrawIndexedEx(primitives[i].mode, primitives[i].numIndices, GX2_INDEX_TYPE_U32, primitives[i].indices, 0, 1);
      }
      WHBGfxFinishRenderDRC();
      WHBGfxFinishRender();
   }

exit:
   WHBLogPrintf("Exiting...");
   WHBGfxShutdown();
   WHBProcShutdown();
   WHBUnmountSdCard();
   return result;
}
