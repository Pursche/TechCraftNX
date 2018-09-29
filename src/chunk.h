#pragma once

#include <vector>

#include <glad/glad.h>

struct Chunk
{
	int32_t x;
	int32_t y;
	int32_t z;

	std::vector<uint32_t> blocks;
};

struct VisualChunk
{
	GLuint vertexArray;
	GLuint positionBuffer;
	GLuint texcoordBuffer;
	GLuint normalBuffer;

	GLuint opaqueIndexBuffer;
	GLsizei opaqueIndexCount;

	GLuint transparentIndexBuffer;
	GLsizei transparentIndexCount;
};

void initChunk(Chunk& chunk, int32_t x, int32_t y, int32_t z);
void initVisualChunk(VisualChunk& visualChunk, const Chunk& chunk);

void drawChunkOpaque(const VisualChunk& chunk);
void drawChunkTransparent(const VisualChunk& chunk);
