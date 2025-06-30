#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec4 color0 = vec4(1, 0, 0, 1);

    float r = 0.05;
    vec2 d = fragTexCoord - vec2(0.5);
    if (length(d) <= 0.5) {
        float m = length(d) - r;
        if (m <= 0) {
            finalColor = color0 * 1.5;
        } else {
            float t = 1 - m / 0.45;
            finalColor = vec4(color0.rgb, t*1.5);
        }
    } else {
        finalColor = vec4(0);
    }
}
