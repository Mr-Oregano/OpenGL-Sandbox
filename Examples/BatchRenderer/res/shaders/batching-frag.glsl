#version 450 core

layout (location = 0) out vec4 o_Color;

uniform vec4 u_Tint;

uniform sampler2D u_TextureSlots[gl_MaxTextureImageUnits];

in vec4 vf_Color;
in vec2 vf_TextureCoords;
in vec2 vf_TexelSize;

flat in int vf_TextureSelection;

void main()
{

	vec2 Texel_TexCoords = vf_TexelSize * vf_TextureCoords;

	vec2 alpha = 0.7 * vec2(dFdx(Texel_TexCoords.x), dFdy(Texel_TexCoords.y));
	vec2 CurrentFragInTexel = fract(Texel_TexCoords);
	vec2 CurrentTexel = floor(Texel_TexCoords);
	vec2 FilterOffset = clamp(CurrentFragInTexel / (2.0 * alpha), 0.0, 0.5)
						+ clamp((CurrentFragInTexel - 1.0) / (2.0 * alpha) + 0.5, 0.0, 0.5);

	vec2 FilteredCoords = (CurrentTexel + FilterOffset) / vf_TexelSize;

	vec4 texColor = vec4(1.0);

	switch (vf_TextureSelection)
	{
	case 0:	
		texColor *= texture(u_TextureSlots[0], FilteredCoords);
		break;
	case 1:	
		texColor *= texture(u_TextureSlots[1], FilteredCoords);
		break;
	case 2:	
		texColor *= texture(u_TextureSlots[2], FilteredCoords);
		break;
	case 3:	
		texColor *= texture(u_TextureSlots[3], FilteredCoords);
		break;
	case 4:	
		texColor *= texture(u_TextureSlots[4], FilteredCoords);
		break;
	case 5:	
		texColor *= texture(u_TextureSlots[5], FilteredCoords);
		break;
	case 6:	
		texColor *= texture(u_TextureSlots[6], FilteredCoords);
		break;
	case 7:	
		texColor *= texture(u_TextureSlots[7], FilteredCoords);
		break;
	case 8:	
		texColor *= texture(u_TextureSlots[8], FilteredCoords);
		break;
	case 9:	
		texColor *= texture(u_TextureSlots[9], FilteredCoords);
		break;
	case 10:	
		texColor *= texture(u_TextureSlots[10], FilteredCoords);
		break;
	case 11:	
		texColor *= texture(u_TextureSlots[11], FilteredCoords);
		break;
	case 12:	
		texColor *= texture(u_TextureSlots[12], FilteredCoords);
		break;
	case 13:	
		texColor *= texture(u_TextureSlots[13], FilteredCoords);
		break;
	case 14:	
		texColor *= texture(u_TextureSlots[14], FilteredCoords);
		break;
	case 15:	
		texColor *= texture(u_TextureSlots[15], FilteredCoords);
		break;
	case 16:	
		texColor *= texture(u_TextureSlots[16], FilteredCoords);
		break;
	case 17:	
		texColor *= texture(u_TextureSlots[17], FilteredCoords);
		break;
	case 18:	
		texColor *= texture(u_TextureSlots[18], FilteredCoords);
		break;
	case 19:	
		texColor *= texture(u_TextureSlots[19], FilteredCoords);
		break;
	case 20:	
		texColor *= texture(u_TextureSlots[20], FilteredCoords);
		break;
	case 21:	
		texColor *= texture(u_TextureSlots[21], FilteredCoords);
		break;
	case 22:	
		texColor *= texture(u_TextureSlots[22], FilteredCoords);
		break;
	case 23:	
		texColor *= texture(u_TextureSlots[23], FilteredCoords);
		break;
	case 24:	
		texColor *= texture(u_TextureSlots[24], FilteredCoords);
		break;
	case 25:	
		texColor *= texture(u_TextureSlots[25], FilteredCoords);
		break;
	case 26:	
		texColor *= texture(u_TextureSlots[26], FilteredCoords);
		break;
	case 27:	
		texColor *= texture(u_TextureSlots[27], FilteredCoords);
		break;
	case 28:	
		texColor *= texture(u_TextureSlots[28], FilteredCoords);
		break;
	case 29:	
		texColor *= texture(u_TextureSlots[29], FilteredCoords);
		break;
	case 30:	
		texColor *= texture(u_TextureSlots[30], FilteredCoords);
		break;
	case 31:	
		texColor *= texture(u_TextureSlots[31], FilteredCoords);
		break;
	}

	o_Color = texColor * vf_Color * u_Tint;

}