#version 450 core

// IEEE-754 floating point standard
// Mask for the mantissa
#define MANTISSA 0x007fffffu

// Integer representation of floating point 1.0
#define IEEE_754_ONE 0x3f800000u

layout(location = 0) out vec4 a_Color;

uniform uint u_AlgorithmType;

uniform vec2 u_Resolution;

uniform float u_Evolve;
uniform uint u_Seed;

uniform float u_Scale = 1.0;
uniform float u_Lacunarity = 1.0;
uniform float u_Persistence = 1.0;
uniform uint  u_Octaves = 1;

uint hash(uint key) 
{
    // Taken from the finalization
    // of the Jenkins "One-at-a-time" hash
    key += key << 10;
    key ^= key >> 6;
    key += key << 3;
    key ^= key >> 11;
    key += key << 15;
    
	return key;
}

float rand(float coord, uint seed) 
{
    uint intBits = uint(floatBitsToUint(coord) ^ seed);
    
    uint hash = hash(intBits);
    uint intRand = (hash & MANTISSA) | IEEE_754_ONE;
    
    return uintBitsToFloat(intRand) - 1.0;
}

uint hash(uvec2 key) 
{
    // Taken from the finalization
    // of the Jenkins "One-at-a-time" hash
    uint hash_out = 0u;

    hash_out += key.x;
    hash_out += hash_out << 10;
    hash_out ^= hash_out >> 6;

    hash_out += key.y;
    hash_out += hash_out << 10;
    hash_out ^= hash_out >> 6;

    hash_out += hash_out << 3;
    hash_out ^= hash_out >> 11;
    hash_out += hash_out << 15;
    
	return hash_out;
}

float rand(vec2 coord, uint seed) 
{
    uvec2 intBits = uvec2(floatBitsToUint(coord) ^ seed);
    
    uint hash = hash(intBits);
    uint intRand = (hash & MANTISSA) | IEEE_754_ONE;
    
    return uintBitsToFloat(intRand) - 1.0;
}

uint hash(uvec3 key) 
{
    // Taken from the finalization
    // of the Jenkins "One-at-a-time" hash
    uint hash_out = 0u;

    hash_out += key.x;
    hash_out += hash_out << 10;
    hash_out ^= hash_out >> 6;

    hash_out += key.y;
    hash_out += hash_out << 10;
    hash_out ^= hash_out >> 6;

    hash_out += key.z;
    hash_out += hash_out << 10;
    hash_out ^= hash_out >> 6;

    hash_out += hash_out << 3;
    hash_out ^= hash_out >> 11;
    hash_out += hash_out << 15;
    
	return hash_out;
}

float rand(vec3 coord, uint seed) 
{
    uvec3 intBits = uvec3(floatBitsToUint(coord) ^ seed);
    
    uint hash = hash(intBits);
    uint intRand = (hash & MANTISSA) | IEEE_754_ONE;
    
    return uintBitsToFloat(intRand) - 1.0;
}

float simplex_noise(vec2 coordinates, uint seed)
{
    // Preset Gradients for each simplex vertex.
    const uint gradients2D_length = 4;
    const vec2 gradients2D[] = vec2[](
        vec2( 1.0,  1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0, -1.0),
        vec2(-1.0,  1.0)
    );

    // Transform the simplex grid into tile-space in order
    // to extract the origin of the current hypercube.
    const float F2 = (sqrt(3.0) - 1.0) / 2.0;
    float s = (coordinates.x + coordinates.y) * F2;
    vec2 tile = floor(coordinates + s);

    // Transform the previously calculated tile coorindate
    // back into x,y space and use it to calculate the displacement
    // vector from the origin vertex to the fragment coordinates.
    const float G2 = (1.0 - 1.0 / sqrt(3)) / 2.0;
    float t = (tile.x + tile.y) * G2;
    vec2 d0 = coordinates - (tile - t);

    // Determine which simplex the fragment coordinates is in.
    // If the displacement vector's 'x > y', then we are in bottom
    // simplex with middle coordinate (1, 0). Otherwise we are in 
    // simplex with middle coordinate of (0, 1).
    float simplex_loc = abs(step(0.0, d0.x - d0.y));
    vec2 middle_corner = vec2(simplex_loc, 1.0 - simplex_loc);

    // Calculate remaining displacement vectors from the other
    // vertices to the fragment coordinates.
    vec2 d1 = d0 - middle_corner + G2;
    vec2 d2 = d0 - 1.0 + 2.0 * G2;

    // Psuedo-randomly select gradients for each of the three simplex vertices.
    vec2 grad0 = gradients2D[int(rand(tile + vec2(0.0, 0.0), seed) * gradients2D_length)];
    vec2 grad1 = gradients2D[int(rand(tile +  middle_corner, seed) * gradients2D_length)];
    vec2 grad2 = gradients2D[int(rand(tile + vec2(1.0, 1.0), seed) * gradients2D_length)];

    // Calculate contributions from each of the 3 corners based off of a multiplication
    // of a radially symmetric attenuation function and the gradient ramp calculated from the
    // scalar product of the displacement vectors and the previously selected gradients.
    float contribution0 = pow(max(0, 0.5 - d0.x * d0.x - d0.y * d0.y), 4) * dot(grad0, d0);
    float contribution1 = pow(max(0, 0.5 - d1.x * d1.x - d1.y * d1.y), 4) * dot(grad1, d1);
    float contribution2 = pow(max(0, 0.5 - d2.x * d2.x - d2.y * d2.y), 4) * dot(grad2, d2);

    // Sum the contributions and normalize the output between [0.0, 1.0)
    return (70.0 * (contribution0 + contribution1 + contribution2) + 1.0) / 2.0;
}

float perlin_noise(vec2 coordinates, uint seed)
{
    const uint gradients2D_length = 4;
    const vec2 gradients2D[] = vec2[](
        vec2( 1.0,  1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0, -1.0),
        vec2(-1.0,  1.0)
    );

    vec2 tile = floor(coordinates);
    vec2 frag = fract(coordinates);

    vec2 grad00 = gradients2D[int(rand(tile + vec2(0.0, 0.0), seed) * gradients2D_length)];
    vec2 grad01 = gradients2D[int(rand(tile + vec2(0.0, 1.0), seed) * gradients2D_length)];
    vec2 grad10 = gradients2D[int(rand(tile + vec2(1.0, 0.0), seed) * gradients2D_length)];
    vec2 grad11 = gradients2D[int(rand(tile + vec2(1.0, 1.0), seed) * gradients2D_length)];

    float rand00 = dot(grad00, frag - vec2(0.0, 0.0));
    float rand01 = dot(grad01, frag - vec2(0.0, 1.0));
    float rand10 = dot(grad10, frag - vec2(1.0, 0.0));
    float rand11 = dot(grad11, frag - vec2(1.0, 1.0));

    float blend_x = smoothstep(0.0, 1.0, frag.x);
    float blend_y = smoothstep(0.0, 1.0, frag.y);

    float randx0 = mix(rand00, rand10, blend_x);
    float randx1 = mix(rand01, rand11, blend_x);

    float randxy = mix(randx0, randx1, blend_y);
    return (randxy + 1.0) / 2.0;
}

float perlin_noise(vec3 coordinates, uint seed)
{
    const uint gradients3D_length = 8;
    const vec3 gradients3D[] = vec3[](
        vec3( 1.0,  1.0,  1.0),
        vec3( 1.0,  1.0, -1.0),
        vec3( 1.0, -1.0,  1.0), 
        vec3(-1.0,  1.0,  1.0),
        vec3( 1.0, -1.0, -1.0),
        vec3(-1.0, -1.0,  1.0),
        vec3(-1.0,  1.0, -1.0),
        vec3(-1.0, -1.0, -1.0)
    );

    vec3 tile = floor(coordinates);
    vec3 frag = fract(coordinates);

    vec3 grad000 = gradients3D[int(rand(tile + vec3(0.0, 0.0, 0.0), seed) * gradients3D_length)];
    vec3 grad010 = gradients3D[int(rand(tile + vec3(0.0, 1.0, 0.0), seed) * gradients3D_length)];
    vec3 grad100 = gradients3D[int(rand(tile + vec3(1.0, 0.0, 0.0), seed) * gradients3D_length)];
    vec3 grad110 = gradients3D[int(rand(tile + vec3(1.0, 1.0, 0.0), seed) * gradients3D_length)];
    vec3 grad001 = gradients3D[int(rand(tile + vec3(0.0, 0.0, 1.0), seed) * gradients3D_length)];
    vec3 grad011 = gradients3D[int(rand(tile + vec3(0.0, 1.0, 1.0), seed) * gradients3D_length)];
    vec3 grad101 = gradients3D[int(rand(tile + vec3(1.0, 0.0, 1.0), seed) * gradients3D_length)];
    vec3 grad111 = gradients3D[int(rand(tile + vec3(1.0, 1.0, 1.0), seed) * gradients3D_length)];

    float rand000 = dot(grad000, frag - vec3(0.0, 0.0, 0.0));
    float rand010 = dot(grad010, frag - vec3(0.0, 1.0, 0.0));
    float rand100 = dot(grad100, frag - vec3(1.0, 0.0, 0.0));
    float rand110 = dot(grad110, frag - vec3(1.0, 1.0, 0.0));
    float rand001 = dot(grad001, frag - vec3(0.0, 0.0, 1.0));
    float rand011 = dot(grad011, frag - vec3(0.0, 1.0, 1.0));
    float rand101 = dot(grad101, frag - vec3(1.0, 0.0, 1.0));
    float rand111 = dot(grad111, frag - vec3(1.0, 1.0, 1.0));

    float blend_x = smoothstep(0.0, 1.0, frag.x);
    float blend_y = smoothstep(0.0, 1.0, frag.y);
    float blend_z = smoothstep(0.0, 1.0, frag.z);

    float randx00 = mix(rand000, rand100, blend_x);
    float randx01 = mix(rand001, rand101, blend_x);
    float randx10 = mix(rand010, rand110, blend_x);
    float randx11 = mix(rand011, rand111, blend_x);

    float randxy0 = mix(randx00, randx10, blend_y);
    float randxy1 = mix(randx01, randx11, blend_y);

    float randxyz = mix(randxy0, randxy1, blend_z);
    const float magnitude = sqrt(9.0 / 4.0);
    return (randxyz + magnitude) / (2.0 * magnitude);
}

float noise(vec3 coordinates, uint seed, float scale, uint octaves, float lacunarity, float persistence)
{
    coordinates *= scale;

    float output_noise = 0.0;
    float frequency = 1.0;
    float amplitude = 1.0;
    float divisor = 0.0;

    for (uint i = 0; i < octaves; ++i)
    {
        divisor += amplitude;

        float noise_val = 0.0;
        switch (u_AlgorithmType)
        {
            case 0: noise_val = simplex_noise(coordinates.xy * frequency, seed + i); break;
            case 1: noise_val = perlin_noise(coordinates.xy * frequency, seed + i); break;
        }

        output_noise += amplitude * noise_val;

        frequency *= lacunarity;
        amplitude *= persistence;
    }
    
    return output_noise / divisor;
}

void main() 
{
    vec2 uv = gl_FragCoord.xy / u_Resolution * 2.0 - 1.0;

    vec3 coordinates = vec3(uv * vec2(u_Resolution.x / u_Resolution.y, 1.0), u_Evolve);

    a_Color = vec4(noise(coordinates, u_Seed, u_Scale, u_Octaves, u_Lacunarity, u_Persistence));
}