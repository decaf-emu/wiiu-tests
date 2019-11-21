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

static const float sPositionData[] =
{
   -0.7f,  0.7f,  0.0f,
    0.7f,  0.7f,  0.0f,
    0.7f, -0.7f,  0.0f,
   -0.7f, -0.7f,  0.0f,
   -0.7f,  0.0f,  0.0f,
    0.7f,  0.0f,  0.0f,
};

static const float sColourData[] =
{
    1.0f,  0.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  0.0f, 1.0f,
    0.0f,  0.0f,  1.0f, 1.0f,
    1.0f,  0.0f,  1.0f, 1.0f,
    1.0f,  1.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  1.0f, 1.0f,
};

static alignas(GX2_INDEX_BUFFER_ALIGNMENT) int
sIndicesTriangles[] = {
    0,  1,  3,
    1,  2,  3,
};

int main(int argc, char **argv)
{
   GX2RBuffer positionBuffer = { 0 };
   GX2RBuffer colourBuffer = { 0 };
   WHBGfxShaderGroup group = { 0 };
   GX2UniformVar *uniformVar = NULL;
   void *buffer = NULL;
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
   sprintf(path, "%s/decaf/shaders/pos_colour_mvp.gsh", sdRootPath);

   gshFileData = WHBReadWholeFile(path, NULL);
   if (!WHBGfxLoadGFDShaderGroup(&group, 0, gshFileData)) {
      result = -1;
      WHBLogPrintf("WHBGfxLoadGFDShaderGroup returned  FALSE");
      goto exit;
   }

   WHBGfxInitShaderAttribute(&group, "position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
   WHBGfxInitShaderAttribute(&group, "colour", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitFetchShader(&group);

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU, sIndicesTriangles, sizeof(sIndicesTriangles));

   WHBFreeWholeFile(gshFileData);
   gshFileData = NULL;

   positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
   positionBuffer.elemSize = 4 * 3;
   positionBuffer.elemCount = 6;
   GX2RCreateBuffer(&positionBuffer);
   buffer = GX2RLockBufferEx(&positionBuffer, 0);
   memcpy(buffer, sPositionData, positionBuffer.elemSize * positionBuffer.elemCount);
   GX2RUnlockBufferEx(&positionBuffer, 0);

   colourBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
   colourBuffer.elemSize = 4 * 4;
   colourBuffer.elemCount = 6;
   GX2RCreateBuffer(&colourBuffer);
   buffer = GX2RLockBufferEx(&colourBuffer, 0);
   memcpy(buffer, sColourData, colourBuffer.elemSize * colourBuffer.elemCount);
   GX2RUnlockBufferEx(&colourBuffer, 0);

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "modelMatrix")) {
      uniformModelMatrixLoc = uniformVar->offset;
   }

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "viewMatrix")) {
      uniformViewMatrixLoc = uniformVar->offset;
   }

   if (uniformVar = GX2GetVertexUniformVar(group.vertexShader, "projectionMatrix")) {
      uniformProjectionMatrixLoc = uniformVar->offset;
   }

   gbMat4 projectionMatrix;
   gbMat4 viewMatrix;
   gbMat4 modelMatrix;
   gb_mat4_ortho3d(&projectionMatrix, -8.0f, 8.0f, -4.5f, 4.5f, 0.0f, 20.0f);

   gbVec3 eye = { 0.0f, 0.0f, 10.0f };
   gbVec3 center = { 0.0f, 0.0f, 0.0f };
   gbVec3 up = { 0.0f, 1.0f, 0.0f };
   gb_mat4_look_at(&viewMatrix, eye, center, up);

   gb_mat4_identity(&modelMatrix);
   gb_mat4_scalef(&modelMatrix, 3.0f);

   gb_mat4_transpose(&viewMatrix);
   gb_mat4_transpose(&modelMatrix);
   gb_mat4_transpose(&projectionMatrix);

   WHBLogPrintf("Begin rendering ...");
   while (WHBProcIsRunning()) {
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(0.5f, 0.0f, 0.0f, 1.0f);
      GX2SetVertexUniformReg(uniformModelMatrixLoc, 16, &modelMatrix);
      GX2SetVertexUniformReg(uniformViewMatrixLoc, 16, &viewMatrix);
      GX2SetVertexUniformReg(uniformProjectionMatrixLoc, 16, &projectionMatrix);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2DrawIndexedEx(GX2_PRIMITIVE_MODE_TRIANGLES, 6, GX2_INDEX_TYPE_U32, sIndicesTriangles, 0, 1);
      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(0.5f, 0.0f, 0.0f, 1.0f);
      GX2SetVertexUniformReg(uniformModelMatrixLoc, 16, &modelMatrix);
      GX2SetVertexUniformReg(uniformViewMatrixLoc, 16, &viewMatrix);
      GX2SetVertexUniformReg(uniformProjectionMatrixLoc, 16, &projectionMatrix);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2DrawIndexedEx(GX2_PRIMITIVE_MODE_TRIANGLES, 6, GX2_INDEX_TYPE_U32, sIndicesTriangles, 0, 1);
      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

exit:
   WHBLogPrintf("Exiting...");
   GX2RDestroyBufferEx(&positionBuffer, 0);
   GX2RDestroyBufferEx(&colourBuffer, 0);
   WHBGfxShutdown();
   WHBProcShutdown();
   WHBUnmountSdCard();
   return result;
}
