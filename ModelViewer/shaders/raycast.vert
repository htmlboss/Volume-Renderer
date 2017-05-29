#version 450
layout (location = 0) in vec3 VerPos;
layout (location = 1) in vec3 VerClr;

// Model-View-Projection
uniform mat4 MVP;

out vec3 EntryPoint;
out vec4 ExitPointCoord;

void main() {
    EntryPoint = VerClr;
    gl_Position = MVP * vec4(VerPos,1.0);
    ExitPointCoord = gl_Position;
}
