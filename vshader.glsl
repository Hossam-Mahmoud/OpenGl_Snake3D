#version 150

in   vec4 vPosition;
in   vec3 vNormal;

// output values that will be interpretated per-fragment
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

uniform mat4 Model;
uniform mat4 Camera;
uniform vec4 LightPosition;
uniform mat4 Projection;

void main()
{
    // Compute model-view matrix
    mat4 ModelView = Camera * Model;

    // Transform vertex  position into eye coordinates
    vec3 pos = ( ModelView * vPosition ).xyz;

    // Compute viewer vector in eye coordinate
    fE = -pos;

    // Compute light vector in eye coordinate
    if( LightPosition.w == 0.0 )
        fL = ( Camera * LightPosition ).xyz;
    else
        fL = ( Camera * LightPosition ).xyz - pos;

    // Transform normal vector into eye coordinates
    fN = ( ModelView * vec4(vNormal, 0) ).xyz;

    gl_Position = Projection * ModelView * vPosition;
}
