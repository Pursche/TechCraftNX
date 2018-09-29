#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

struct Chunk
{
	int32_t x;
	int32_t y;
	int32_t z;

	std::vector<uint32_t> blocks;
};

struct VisualChunk
{
	static bool s_triangleFilteringEnabled;
	static bool s_freezeCulling;

	static void init();
	static void deinit();

	GLuint vertexArray;
	GLuint positionBuffer;
	GLuint texcoordBuffer;
	GLuint normalBuffer;

	GLuint opaqueIndexBuffer;
	GLsizei opaqueIndexCount;
	GLuint culledOpaqueIndexBuffer;
	GLuint opaqueDrawArgs;

	GLuint transparentIndexBuffer;
	GLsizei transparentIndexCount;
	GLuint culledTransparentIndexBuffer;
	GLuint transparentDrawArgs;
};

void initChunk(Chunk& chunk, int32_t x, int32_t y, int32_t z);
void initVisualChunk(VisualChunk& visualChunk, const Chunk& chunk);

struct CullChunkParams
{
	glm::mat4 matViewProj;
};

void cullChunk(const VisualChunk& chunk, const CullChunkParams& params);
void drawChunkOpaque(const VisualChunk& chunk);
void drawChunkTransparent(const VisualChunk& chunk);
