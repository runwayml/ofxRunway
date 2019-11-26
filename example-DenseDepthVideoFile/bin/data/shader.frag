#version 120
#extension GL_EXT_gpu_shader4 : require

uniform sampler2DRect video;				
// uniform sampler2DRect depth;				

varying vec2 texCoordVarying;
// uniform float pointStride;


void main()
{
    // gl_FragColor = texture2DRect(video, texCoordVarying);
    // if(mod(texCoordVarying.x, floor(pointStride)) > 1.f ||
    //    mod(texCoordVarying.y, floor(pointStride)) > 1.f){
    // 	gl_FragColor = vec4(0.,0.,0.,0.);
    // }else{
	    gl_FragColor = texture2DRect(video, texCoordVarying);
    	// gl_FragColor = vec4(texCoordVarying,0.,1.0);
    // gl_FragColor = vec4(1.0,0.,0.,1.0);
	// }
}
