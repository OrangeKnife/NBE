#pragma once
#define uint unsigned int
#include <vector>
#include "Vertex.h"
#include "NBE.h"
namespace NBE
{
    enum ATTRI
    {
        POS = 0,
        TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
        NORMAL,
		COLOR,
		TANGENT,
		BITANGENT,
		BONE,
		WEIGHT,
        SumOfAttri
    };

	const std::string ATTRI_Names[SumOfAttri]=
	{
		"aPOS",
		"aTEXCOORD0",
		"aTEXCOORD1",
		"aTEXCOORD2",
		"aNORMAL",
		"aCOLOR",
		"aTANGENT",
		"aBITANGENT",
		"aBONE",
		"aWEIGHT"
	};

    class NBE_API Shader
    {
    public:
        struct AttriInfo
        {
            uint id;
            int sizeOfAttri;
            uint typeOfAttri;
            AttriInfo(uint _id, int size, uint type)
                :id(_id),sizeOfAttri(size),typeOfAttri(type)
            {}
        };

        inline std::vector<AttriInfo>& getAttributesList()
        {return attriList;}

        Shader()
		{}
        virtual ~Shader()
        {}
 
		virtual void bindAttribute()
		{}
        unsigned int idx;

	protected:
        std::vector<AttriInfo> attriList; 
    };
}