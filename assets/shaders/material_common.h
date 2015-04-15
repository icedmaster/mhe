struct PhongMaterial
{
	vec4 diffuse;
	vec4 specular; // w - shininess
	vec4 params; // x - glossiness
};

#define PHONG_MATERIAL_SHININESS(material) material.specular.w
#define PHONG_MATERIAL_GLOSSINESS(material) material.params.x