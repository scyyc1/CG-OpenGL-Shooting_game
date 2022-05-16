#version 330 core
out vec4 FragColor;

struct payload{
    vec3 LightPos;
    vec3 ViewPos;
    vec3 FragPos;
    vec3 Normal;
};

struct Material {
    bool isAmbientMap;
    bool isDiffuseMap;
    bool isSpecularMap;
    bool isBumpMap;
    sampler2D texture_ambient;
    sampler2D texture_diffuse;
    sampler2D texture_specular; 
    sampler2D texture_bump;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;
    // Represent 3 type of Light
    // 0 for Directional, 1 for positional and 2 for spot
    int type;

    // The colour of light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;

    vec3 Colour;

    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light lamp;
uniform Light torch;
uniform float cutoff_cone;
uniform bool isTorchUsed;

vec3 calculateAmbient(Light light) {
    vec3 ambient = light.ambient;

    if(material.isAmbientMap)
        ambient = light.ambient * texture(material.texture_ambient, fs_in.TexCoords).rgb;
    else
        ambient = light.ambient * fs_in.Colour;
    return ambient;
}

vec3 calculateDiffuse(Light light, payload elements) {
    vec3 normal = normalize(elements.Normal);
    vec3 lightDir = normalize(elements.LightPos - elements.FragPos);
    float diff = max(dot(normal, lightDir), 0.f);

    vec3 diffuse = light.diffuse * diff;

    if(material.isDiffuseMap)
        diffuse = diffuse * texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    else
        diffuse = diffuse * fs_in.Colour;
    
    return diffuse;
}

vec3 calculateSpecular(Light light, payload elements) {
    vec3 normal = normalize(elements.Normal);
    vec3 viewDir = normalize(elements.ViewPos - elements.FragPos);
    vec3 lightDir = normalize(elements.LightPos - elements.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
//    vec3 halfwayDir = normalize(lightDir + viewDir); 
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 128);
    vec3 specular = light.specular * spec;
    
    if(material.isSpecularMap)
        specular = specular * texture(material.texture_specular, fs_in.TexCoords).rgb; 
    else
        specular = specular * fs_in.Colour;

    return specular;
}

float calculateAttenuation(payload elements) {
    float d = length(elements.LightPos - elements.FragPos);
	float att_c = 1.5f, att_l = 0.05, att_q = 0.02;
	float attenuation = 1 / (att_c + (att_l * d) + (att_q * pow(d, 2)));

    return attenuation;
}

vec4 phongShading (payload elements, Light light)
{
    // ambient
    vec3 ambient = calculateAmbient(light);
    // diffuse 
    vec3 diffuse = calculateDiffuse(light, elements);  
    // specular
    vec3 specular = calculateSpecular(light, elements);

    return vec4(diffuse + specular + ambient, 1.0);
}

vec4 calculatePositionalLight(payload elements, Light light){
    vec4 phong = phongShading(elements, light);
    return phong * calculateAttenuation(elements);
}

vec4 calculateSpotLight(payload elements, Light light){
    vec4 phong = phongShading(elements, light);
    phong = phong * calculateAttenuation(elements);

    // cutoff cone
	float cutoff_cone = cos(radians(15));
	vec3 spotDir = normalize(light.direction);
    vec3 lightDir = -normalize(elements.LightPos - elements.FragPos);
	float theta = dot(lightDir, spotDir);

	// Phong
	if(theta > cutoff_cone){
		phong = phong;
	}else{
		phong = vec4(calculateAmbient(light), 1.f) * 0;
	}
    return phong;
}

// Not use
//vec4 withNormalMapping()
//{
//    // bump mapping
//    vec3 normal = texture(material.texture_bump, fs_in.TexCoords).rgb;
//    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
//
//    // ambient
//    vec3 ambient = lamp.ambient * texture(material.texture_ambient, fs_in.TexCoords).rgb;
//  	
//    // diffuse 
//    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = lamp.diffuse * diff * texture(material.texture_diffuse, fs_in.TexCoords).rgb;  
//    
//    // specular
//    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
//    vec3 reflectDir = reflect(-lightDir, normal);  
//    vec3 halfwayDir = normalize(lightDir + viewDir); 
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
//    vec3 specular = lamp.specular * spec * texture(material.texture_specular, fs_in.TexCoords).rgb; 
//
//    return vec4(diffuse + specular + ambient, 1.f);
//} 

payload generatePayload(Light light) {
    payload elements;
    if(true){
        elements.ViewPos = viewPos;
        elements.FragPos = fs_in.FragPos;
        elements.LightPos = light.position;
        elements.Normal = fs_in.Normal;
    }
//    else{
//        vec3 normal = texture(material.texture_bump, fs_in.TexCoords).rgb;
//        normal = normalize(normal * 2.0 - 1.0);
//        elements.ViewPos = fs_in.TangentViewPos;
//        elements.FragPos = fs_in.TangentFragPos;
//        elements.LightPos = fs_in.TangentLightPos;
//        elements.Normal = normal;
//    }
    return elements;
}

void main()
{
    payload torchPayload = generatePayload(torch);
    payload lampPayload = generatePayload(lamp);
    
    FragColor = calculatePositionalLight(lampPayload, lamp);

    if(isTorchUsed)
        FragColor = FragColor + calculateSpotLight(torchPayload, torch); 
} 