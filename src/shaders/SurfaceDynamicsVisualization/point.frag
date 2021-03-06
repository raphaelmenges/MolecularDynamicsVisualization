//============================================================================
// Distributed under the MIT License. Author: Raphael Menges
//============================================================================

#version 430

layout(location = 0) out vec4 fragColor;

uniform vec4 color;

void main()
{
    fragColor = color;
}
