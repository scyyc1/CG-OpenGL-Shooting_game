#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1; 
    sampler2D texture_ambient1;
    sampler2D texture_bump1;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec4 withoutNormalMapping ()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.texture_ambient1, fs_in.TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb; 

    return vec4(diffuse + specular + ambient, 1.0);
}

vec4 withNormalMapping()
{
    // bump mapping
    vec3 normal = texture(material.texture_bump1, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_ambient1, fs_in.TexCoords).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb; 

    return vec4(diffuse + specular + ambient, 1.f);
} 

void main()
{
    FragColor = withNormalMapping();
//    FragColor = withoutNormalMapping(); 
} 