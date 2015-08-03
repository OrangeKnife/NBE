uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

attribute vec3 aPOS;
attribute vec2 aTEXCOORD0;
attribute vec3 aNORMAL;
attribute vec4 aCOLOR;
attribute vec3 aTANGENT;
attribute vec3 aBITANGENT;
attribute vec4 aBONE;
attribute vec3 aWEIGHT;

varying vec2 texcoord0;
varying vec4 color;
 
void main()
{
   gl_Position = projMat * viewMat * modelMat *  vec4(aPOS,1.0);
   texcoord0 = aTEXCOORD0;
   color = aCOLOR;
}

