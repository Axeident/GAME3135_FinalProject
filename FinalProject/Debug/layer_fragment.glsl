#version 150

in mat4 modelview;

in vec3 frag_position, frag_normal;
in vec2 frag_uv;

uniform	vec4 color;
uniform	float shininess = 1;
uniform	vec3 specular_color = vec3(0);
uniform mat4 camera;

uniform sampler2D surface;

uniform vec3 light_position = vec3(-3, 5, -4);
uniform vec3 ambient_light = vec3(0.20);

out vec4 fragColor;

vec4 highlight(vec3 direction, vec3 position, vec3 normal, float shiny) {
	direction = reflect(direction, normal);
	float nearness = max(0.0, dot(normalize(-position), normalize(direction)));
	return vec4(specular_color, pow(nearness, max(1, shiny)));
}

void main() {
    vec3 normal = normalize(frag_normal);
	vec4 albedo = color * texture(surface, frag_uv);
	vec3 light_direction = normalize(frag_position - vec3(camera * vec4(light_position, 1.0)));
    float shade = max(0, dot(normal, -light_direction));
	vec3 diffuse = albedo.rgb * shade; 
	if (shade > 0.0) {
		vec4 spot = highlight(light_direction, frag_position, normal, shininess);
		diffuse = mix(diffuse, vec3(1.0), spot.rgb * spot.a);
//		fragColor = spot; return;
	} 
	diffuse = mix(diffuse, vec3(1.0), ambient_light);
    fragColor = vec4(diffuse, albedo.a);
}