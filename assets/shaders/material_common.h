struct PhongMaterial
{
	vec4 diffuse;
	vec4 specular; // w - shininess
};

#define PHONG_MATERIAL_SHININESS(material) material.specular.w
