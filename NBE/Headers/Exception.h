#pragma once
#define NException(A,B) NBEException((A),(B),__FILE__,__LINE__)
namespace NBE
{
    //remember define your own 
    enum ERRORCODE
    {
        RenderSysError = 0,

        CharToWcharOverFlow = 50,

        CantFindConfigItem = 100,
        ReadFileError = 150,

        CantGetFuncInDll = 200,

        CreateWindowError = 250,

        TextureDevilError = 300,

        VertexShaderError = 350,
        PixelShaderError,
        LinkShaderError,

		MeshLoadError = 400,


    };

    class NBEException
    {
    public:
        NBEException(int errorCode, String& sth = String(), char* file = nullptr, int line = 0)
			:m_errorCode(errorCode)
			,something(sth)
        {}
        int ErrorCode(){return m_errorCode;}
		String& getInfo(){return something;}
    private:
        int m_errorCode;
		String something;//want to say something when throw?
		char* file;
		int line;
    };
}