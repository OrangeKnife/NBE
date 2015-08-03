#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace NBE
{
#pragma region VertexFormat
	struct Vertex
	{};

    struct PTN_Vertex:Vertex
    {
        vec3f pos;
        vec2f texcoord;
        vec3f normal;

        PTN_Vertex( vec3f _pos=vec3f(),vec2f _coord=vec2f(),vec3f _normal=vec3f() )
            :pos(_pos)
			,texcoord(_coord)
			,normal(_normal)
        {}
    };

	struct PTC_Vertex:Vertex
	{
		vec3f position;
		vec2f texCoord;
		vec4f color;
		PTC_Vertex(vec3f& pos=vec3f(), vec2f& tex=vec2f(), vec4f& col=vec4f())
			:position(pos)
			,texCoord(tex)
			,color(col)
		{}
	};

	struct PTNC_Vertex:Vertex
	{
		vec3f pos;
		vec2f texcoord;
		vec3f normal;
		vec4f color;

		PTNC_Vertex(vec3f _pos=vec3f(),vec2f _coord=vec2f(),
					vec3f _normal=vec3f(),vec4f _color=vec4f(0,0,0,1))
			:pos(_pos)
			,texcoord(_coord)
			,normal(_normal)
			,color(_color)
		{}
	};

	struct SkinVertex:Vertex
	{
		vec3f pos;
		vec2f texcoord;
		vec3f tangent;
		vec3f bitangent;
		vec3f normal;
		vec4f color;
		vec4<int> boneIdx;
		vec4f weight;

		SkinVertex(vec3f _pos=vec3f(),vec2f _coord=vec2f(), vec3f _normal=vec3f(),vec4f _color=vec4f(0,0,0,1),
			 vec3f _tan = vec3f(), vec3f _bi = vec3f(),vec4<int> _boneIdx = vec4<int>(-1,-1,-1,-1), vec4f _wt = vec4f())
			:pos(_pos)
			,texcoord(_coord)
			,normal(_normal)
			,color(_color)
			,tangent(_tan)
			,bitangent(_bi)
			,boneIdx(_boneIdx)
			,weight(_wt)
		{}
	};
#pragma endregion


}