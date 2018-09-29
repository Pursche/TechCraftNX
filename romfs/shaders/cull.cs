#version 430

layout(std430, binding = 0) readonly buffer inputIndexBuffer
{
    uint g_indices[];
};

layout(std430, binding = 1) writeonly buffer outputIndexBuffer
{
    uint g_outIndices[];
};

layout(std430, binding = 2) readonly buffer positionBuffer
{
    float g_positions[];
};

struct DrawElementsIndirectCommand
{
    uint count;
    uint primCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

layout(std430, binding = 3) buffer indirectArgBuffer
{
    DrawElementsIndirectCommand g_indirectArgs;
};

layout(location = 0) uniform uint g_quadCount;
layout(location = 1) uniform mat4 g_matViewProj;

void unpackIndices(uint packedIndices, out uint first, out uint second)
{
    first = packedIndices >> 16;
    second = packedIndices & 0xffffu;
}

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
void main()
{
    if (gl_GlobalInvocationID.x == 0)
    {
        g_indirectArgs.count = 0;
        g_indirectArgs.primCount = 1;
        g_indirectArgs.firstIndex = 0;
        g_indirectArgs.baseVertex = 0;
        g_indirectArgs.baseInstance = 0;
    }

    uint quadId = gl_GlobalInvocationID.x;
    if (quadId < g_quadCount)
    {
        uint packedIndices0 = g_indices[quadId * 3 + 0];
        uint packedIndices1 = g_indices[quadId * 3 + 1];
        uint packedIndices2 = g_indices[quadId * 3 + 2];

        uint indices[6];
        unpackIndices(packedIndices0, indices[0], indices[1]);
        unpackIndices(packedIndices1, indices[2], indices[3]);
        unpackIndices(packedIndices2, indices[4], indices[5]);

        vec3 vertexPos0 = vec3(
            g_positions[indices[0] * 3 + 0],
            g_positions[indices[0] * 3 + 1],
            g_positions[indices[0] * 3 + 2]
        );
        vec3 vertexPos1 = vec3(
            g_positions[indices[1] * 3 + 0],
            g_positions[indices[1] * 3 + 1],
            g_positions[indices[1] * 3 + 2]
        );
        vec3 vertexPos2 = vec3(
            g_positions[indices[2] * 3 + 0],
            g_positions[indices[2] * 3 + 1],
            g_positions[indices[2] * 3 + 2]
        );

        vec4 ndc0 = g_matViewProj * vec4(vertexPos0, 1);
        vec4 ndc1 = g_matViewProj * vec4(vertexPos1, 1);
        vec4 ndc2 = g_matViewProj * vec4(vertexPos2, 1);

        vec2 ndcMin = min(min(ndc0.xy, ndc1.xy), ndc2.xy);
        vec2 ndcMax = max(max(ndc0.xy, ndc1.xy), ndc2.xy);

        bool isCulled = ndcMax.x < 0 || ndcMax.y < 0 || ndcMin.x > 0 || ndcMin.y > 0;

        isCulled = vertexPos0.x > 0.0f;

        if (!isCulled)
        {
            uint outBaseIndex = atomicAdd(g_indirectArgs.count, 6);
            uint outBufferIndex = outBaseIndex / 2;
            g_outIndices[outBufferIndex + 0] = packedIndices0;
            g_outIndices[outBufferIndex + 1] = packedIndices1;
            g_outIndices[outBufferIndex + 2] = packedIndices2;
        }
    }
}

