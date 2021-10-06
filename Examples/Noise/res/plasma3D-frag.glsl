#version 330 core

#define IEEE_MANTISSA 0x007FFFFFu
#define IEEE_ONE 	  0x3F800000u

out vec4 frag_color;

uniform uint u_Seed;
uniform float u_Time;
uniform float u_Gradients[12 * 3];

uniform vec3 u_Color = vec3(1.0);

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash(uint x) 
{
	x += u_Seed;
	
    x += x << 10u;
    x ^= x >> 6u;
    x += x << 3u;
    x ^= x >> 11u;
    x += x << 15u;
    
    return x;
}
uint hash(uvec2 v) { return hash( v.x ^ hash(v.y)                         ); }
uint hash(uvec3 v) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash(uvec4 v) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct(uint m) 
{
    m &= IEEE_MANTISSA;                     // Keep only mantissa bits (fractional part)
    m |= IEEE_ONE;                          // Add fractional part to 1.0

    return uintBitsToFloat(m) - 1.0;       // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random(float x) { return floatConstruct(hash(floatBitsToUint(x))); }
float random(vec2  v) { return floatConstruct(hash(floatBitsToUint(v))); }
float random(vec3  v) { return floatConstruct(hash(floatBitsToUint(v))); }
float random(vec4  v) { return floatConstruct(hash(floatBitsToUint(v))); }

float fade(float x) 
{
	return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

float getPerlinNoise(float x, float y, float z, float frequency) 
{
	x /= frequency;
	y /= frequency;
	z /= frequency;
	
	vec3 ijk = vec3(floor(x), floor(y), floor(z));
	
	float u = fract(x);
	float v = fract(y);
	float w = fract(z);
	
	int gi000 = int(random(ijk + vec3(0.0, 0.0, 0.0)) * 12.0) * 3;
	int gi001 = int(random(ijk + vec3(0.0, 0.0, 1.0)) * 12.0) * 3;
	int gi010 = int(random(ijk + vec3(0.0, 1.0, 0.0)) * 12.0) * 3;
	int gi011 = int(random(ijk + vec3(0.0, 1.0, 1.0)) * 12.0) * 3;
	int gi100 = int(random(ijk + vec3(1.0, 0.0, 0.0)) * 12.0) * 3;
	int gi101 = int(random(ijk + vec3(1.0, 0.0, 1.0)) * 12.0) * 3;
	int gi110 = int(random(ijk + vec3(1.0, 1.0, 0.0)) * 12.0) * 3;
	int gi111 = int(random(ijk + vec3(1.0, 1.0, 1.0)) * 12.0) * 3;
	
	float n000 = dot(vec3(u_Gradients[gi000], u_Gradients[gi000 + 1], u_Gradients[gi000 + 2]), vec3(u - 0, v - 0, w - 0));
	float n100 = dot(vec3(u_Gradients[gi100], u_Gradients[gi100 + 1], u_Gradients[gi100 + 2]), vec3(u - 1, v - 0, w - 0));
	float n010 = dot(vec3(u_Gradients[gi010], u_Gradients[gi010 + 1], u_Gradients[gi010 + 2]), vec3(u - 0, v - 1, w - 0));
	float n110 = dot(vec3(u_Gradients[gi110], u_Gradients[gi110 + 1], u_Gradients[gi110 + 2]), vec3(u - 1, v - 1, w - 0));
	float n001 = dot(vec3(u_Gradients[gi001], u_Gradients[gi001 + 1], u_Gradients[gi001 + 2]), vec3(u - 0, v - 0, w - 1));
	float n101 = dot(vec3(u_Gradients[gi101], u_Gradients[gi101 + 1], u_Gradients[gi101 + 2]), vec3(u - 1, v - 0, w - 1));
	float n011 = dot(vec3(u_Gradients[gi011], u_Gradients[gi011 + 1], u_Gradients[gi011 + 2]), vec3(u - 0, v - 1, w - 1));
	float n111 = dot(vec3(u_Gradients[gi111], u_Gradients[gi111 + 1], u_Gradients[gi111 + 2]), vec3(u - 1, v - 1, w - 1));
	
	float nx00 = mix(n000, n100, fade(u));
	float nx01 = mix(n001, n101, fade(u));
	float nx10 = mix(n010, n110, fade(u));
	float nx11 = mix(n011, n111, fade(u));
	
	float nxy0 = mix(nx00, nx10, fade(v));
	float nxy1 = mix(nx01, nx11, fade(v));
	
	return mix(nxy0, nxy1, fade(w));
}

float getNoise(vec3 v, int octaves, float lacunarity, float gain) 
{
	float finalOutput = 0.0;
	float divisor = 0.0;
	
	float amplitude = 1.0;
	float frequency = 256;
	
	for (int i = 0; i < octaves; ++i) {
		finalOutput += amplitude * abs(getPerlinNoise(v.x, v.y, v.z, frequency));
		divisor += amplitude;
		
		frequency /= lacunarity;
		amplitude *= gain;
	}
	
	finalOutput /= divisor;	
	return pow(1.0 - finalOutput, 3.0);
}

void main() {
	
	vec3 noise_pos = vec3(gl_FragCoord.xy, u_Time * 300.0);
	float plot = getNoise(noise_pos, 7, 2.0, 0.5);
	
	vec3 luma = u_Color * plot + smoothstep(0.5, 1.0, plot);
	frag_color = vec4(luma, 1.0);
	
}
