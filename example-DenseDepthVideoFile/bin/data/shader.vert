#version 120
#extension GL_EXT_gpu_shader4 : require

uniform sampler2DRect video;                
uniform sampler2DRect depth;                
uniform float depth_scale;
uniform float pointSize;
uniform vec2 res;
uniform vec2 img_size;
// uniform float stride;

varying vec2 texCoordVarying;


void main()
{    

//     float instanceX =   mod(gl_InstanceID,x_res);///x_res;// - x_res/2.0);
//     float instanceY = floor(gl_InstanceID/x_res);//y_res; //;- y_res/2.0);

//     vec4 pos = gl_Vertex * pointSize;
//     pos.x += instanceX * stride;
//     pos.y += instanceY * stride;

//     texCoordVarying = vec2(instanceX/x_res, instanceY/y_res);
    
//     // pos.z += texture2DRect(depth, texCoordVarying).g * depth_scale;

       

//     // finally set the pos to be that actual position rendered
//     gl_Position = gl_ModelViewProjectionMatrix * pos;
// }

    
    
    vec2 instance;
    instance.x = mod(gl_InstanceID,res.x);// / res.x - 0.5;
    instance.y = floor(gl_InstanceID/res.x);// / res.y - 0.5;
    
    instance /= res; //normalize
    
    texCoordVarying = instance * img_size;

    instance -= 0.5; // Translate so center of all instances is at {0,0}

    
    
    
    
    vec4 vPos = gl_Vertex;//                   * pointSize;
    vPos.xyz = vPos.xyz * pointSize;


    // vPos.z += 

    vPos = vPos + vec4(instance.x * img_size.x, instance.y * -img_size.y , -texture2DRect(depth, texCoordVarying).g * depth_scale,0.0);
    

    gl_Position = gl_ModelViewProjectionMatrix * vPos  ;
}