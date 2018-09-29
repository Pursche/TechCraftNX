#include "chunk.h"

#include <random>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

constexpr size_t ChunkWidth = 16;
constexpr size_t ChunkHeight = 16;
constexpr size_t ChunkDepth = 16;

constexpr size_t ChunkVoxelCount = ChunkWidth * ChunkHeight * ChunkDepth;

void initChunk(Chunk& chunk, int32_t x, int32_t y, int32_t z)
{
	chunk.x = x;
	chunk.y = y;
	chunk.z = z;

	chunk.blocks.resize(ChunkVoxelCount);

	//const int32_t chunkX = x * ChunkWidth;
	//const int32_t chunkY = y * ChunkHeight;
	//const int32_t chunkZ = z * ChunkDepth;

	std::mt19937 random;
	std::bernoulli_distribution dist(0.1);

	for (int32_t voxelIt = 0; voxelIt < static_cast<int32_t>(ChunkVoxelCount); ++voxelIt)
	{
		//const int32_t voxelX = chunkX + (voxelIt % ChunkWidth);
		//const int32_t voxelY = chunkY + ((voxelIt / ChunkWidth) % ChunkHeight);
		//const int32_t voxelZ = chunkZ + ((voxelIt / ChunkWidth) / ChunkHeight);

		chunk.blocks[voxelIt] = dist(random);
	}
}

void initVisualChunk(VisualChunk& visualChunk, const Chunk& chunk)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;
	std::vector<uint16_t> opaqueIndices;
	std::vector<uint16_t> transparentIndices;

	const int32_t chunkX = chunk.x * ChunkWidth;
	const int32_t chunkY = chunk.y * ChunkHeight;
	const int32_t chunkZ = chunk.z * ChunkDepth;

	for (size_t voxelIt = 0; voxelIt < ChunkVoxelCount; ++voxelIt)
	{
		if (chunk.blocks[voxelIt] != 0)
		{
			const int32_t voxelX = chunkX + (voxelIt % ChunkWidth);
			const int32_t voxelY = chunkY + ((voxelIt / ChunkWidth) % ChunkHeight);
			const int32_t voxelZ = chunkZ + ((voxelIt / ChunkWidth) / ChunkHeight);

			const glm::vec3 pos(static_cast<float>(voxelX), static_cast<float>(voxelY), static_cast<float>(voxelZ));

			uint16_t baseVertex;

			bool isSemitransparent = false;

			std::vector<uint16_t>& indices = isSemitransparent ? transparentIndices : opaqueIndices;

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(0, 0, 0));
			positions.push_back(pos + glm::vec3(1, 0, 0));
			positions.push_back(pos + glm::vec3(0, 1, 0));
			positions.push_back(pos + glm::vec3(1, 1, 0));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(0, 0, 1));
			positions.push_back(pos + glm::vec3(1, 0, 1));
			positions.push_back(pos + glm::vec3(0, 1, 1));
			positions.push_back(pos + glm::vec3(1, 1, 1));
			normals.push_back(glm::vec3(0, 0, -1));
			normals.push_back(glm::vec3(0, 0, -1));
			normals.push_back(glm::vec3(0, 0, -1));
			normals.push_back(glm::vec3(0, 0, -1));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(0, 0, 0));
			positions.push_back(pos + glm::vec3(1, 0, 0));
			positions.push_back(pos + glm::vec3(0, 0, 1));
			positions.push_back(pos + glm::vec3(1, 0, 1));
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));
			normals.push_back(glm::vec3(0, 1, 0));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(0, 1, 0));
			positions.push_back(pos + glm::vec3(1, 1, 0));
			positions.push_back(pos + glm::vec3(0, 1, 1));
			positions.push_back(pos + glm::vec3(1, 1, 1));
			normals.push_back(glm::vec3(0, -1, 0));
			normals.push_back(glm::vec3(0, -1, 0));
			normals.push_back(glm::vec3(0, -1, 0));
			normals.push_back(glm::vec3(0, -1, 0));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(0, 0, 0));
			positions.push_back(pos + glm::vec3(0, 1, 0));
			positions.push_back(pos + glm::vec3(0, 0, 1));
			positions.push_back(pos + glm::vec3(0, 1, 1));
			normals.push_back(glm::vec3(1, 0, 0));
			normals.push_back(glm::vec3(1, 0, 0));
			normals.push_back(glm::vec3(1, 0, 0));
			normals.push_back(glm::vec3(1, 0, 0));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);

			baseVertex = static_cast<uint16_t>(positions.size());
			positions.push_back(pos + glm::vec3(1, 0, 0));
			positions.push_back(pos + glm::vec3(1, 1, 0));
			positions.push_back(pos + glm::vec3(1, 0, 1));
			positions.push_back(pos + glm::vec3(1, 1, 1));
			normals.push_back(glm::vec3(-1, 0, 0));
			normals.push_back(glm::vec3(-1, 0, 0));
			normals.push_back(glm::vec3(-1, 0, 0));
			normals.push_back(glm::vec3(-1, 0, 0));
			indices.push_back(baseVertex + 0);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 2);
			indices.push_back(baseVertex + 1);
			indices.push_back(baseVertex + 3);
		}
	}

	visualChunk.opaqueIndexCount = static_cast<GLsizei>(opaqueIndices.size());
	visualChunk.transparentIndexCount = static_cast<GLsizei>(transparentIndices.size());

	glGenBuffers(1, &visualChunk.positionBuffer);
	glGenBuffers(1, &visualChunk.texcoordBuffer);
	glGenBuffers(1, &visualChunk.normalBuffer);

	glGenBuffers(1, &visualChunk.opaqueIndexBuffer);
	glGenBuffers(1, &visualChunk.transparentIndexBuffer);

	glGenVertexArrays(1, &visualChunk.vertexArray);
	glBindVertexArray(visualChunk.vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, visualChunk.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, visualChunk.texcoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, visualChunk.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualChunk.opaqueIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, opaqueIndices.size() * sizeof(uint16_t), opaqueIndices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualChunk.transparentIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparentIndices.size() * sizeof(uint16_t), transparentIndices.data(), GL_STATIC_DRAW);
}

void drawChunkOpaque(const VisualChunk& chunk)
{
	if (chunk.opaqueIndexCount == 0)
	{
		return;
	}

	glBindVertexArray(chunk.vertexArray);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.opaqueIndexBuffer);
	glDrawElements(GL_TRIANGLES, chunk.opaqueIndexCount, GL_UNSIGNED_SHORT, nullptr);
}

void drawChunkTransparent(const VisualChunk& chunk)
{
	if (chunk.transparentIndexCount == 0)
	{
		return;
	}

	glBindVertexArray(chunk.vertexArray);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.transparentIndexBuffer);
	glDrawElements(GL_TRIANGLES, chunk.transparentIndexCount, GL_UNSIGNED_SHORT, nullptr);
}