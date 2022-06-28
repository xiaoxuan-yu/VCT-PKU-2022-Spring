#version 450 core
// vertex attributes
in vec2 UV;
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec3 WorldTangent;
in vec3 WorldBitangent;
in vec3 WorldLookAtDir;

// Material
uniform float Shininess;
uniform float Opacity;
// Texture
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D MaskTexture;
uniform sampler2D HeightTexture;
uniform vec2 HeightTextureSize;
uniform samplerCube ShadowMap;
uniform int shadowMapRes;
uniform vec3 LightPos;
uniform float far_plane;
uniform float ambientFactor;
// Voxel Const
uniform sampler3D VoxelTexture;
uniform float VoxelGridWorldSize;   //150.f
uniform int VoxelDimensions;    //512

// Output
out vec4 frag_color;

const float MAX_DIST = 75.0;  //max dist
const float ALPHA_THRESH = 0.95; 

//Cone
const int NUM_CONES = 6; // 60 degrees
float coneWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);//Cone Weights
vec3 coneDirections[6] = vec3[]
                            ( 
                            vec3(0, 0, 1),
                            vec3(0, 0.866025,0.5),
                            vec3(0.823639, 0.267617, 0.5),
                            vec3(0.509037, -0.700629, 0.5),
                            vec3(-0.509037, -0.700629, 0.5),
                            vec3(-0.823639, 0.267617, 0.5)
                            );

vec4 sampleVoxels(vec3 world_Position, float lod) {
    vec3 voxelTextureUV = world_Position / (VoxelGridWorldSize * 0.5);  
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

vec4 coneTrace(vec3 direction, float tanHalfAngle) {
    //initialize
    float lod = 0.0; //mipmap level
    vec3 color_ = vec3(0);
    float alpha = 0.0;
    float occlusion = 0.0;

    float voxelWorldSize = VoxelGridWorldSize / VoxelDimensions;  //Voxel len
    float dist = voxelWorldSize; // avoid self-occlusion
    vec3 startPos = WorldPosition + WorldNormal * voxelWorldSize;

    while(dist < MAX_DIST && alpha < ALPHA_THRESH ) {
        // min diameter
        float diameter = max(voxelWorldSize, 2.0 * tanHalfAngle * dist);
        float lodLevel = log2(diameter / voxelWorldSize);
        vec4 voxelColor = sampleVoxels(startPos + dist * direction, lodLevel);

        // accumulate
        color_ +=  (1.0 - alpha) * voxelColor.rgb;
        occlusion += ( (1.0 - alpha) * voxelColor.a) / (1.0 + 0.03 * diameter);
        alpha +=  (1.0 - alpha) * voxelColor.a;
        dist += diameter; 
    }

    return vec4(color_, occlusion);
}

//由高度图计算法线
mat3 TBN;//切线空间变换到世界空间
vec3 calcBumpNormal() {
    // 计算x,y轴梯度
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float curr = texture(HeightTexture, UV).r;
    float diffX = texture(HeightTexture, UV + vec2(offset.x, 0.0)).r - curr;
    float diffY = texture(HeightTexture, UV + vec2(0.0, offset.y)).r - curr;
    //叉乘计算法线
    vec3 t1 = normalize(vec3(1,0,diffX));
    vec3 t2 = normalize(vec3(0,1,diffY));
    vec3 bumpNormal = normalize(cross(t1,t2));
    //映射到世界空间
    return normalize(TBN * bumpNormal);
}

float countShadow(float bias)
{
	vec3 fragToLight = WorldPosition - LightPos;
    float currentDepth = length(fragToLight);

	float shadow = 0.0;
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(ShadowMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= far_plane;   // Undo mapping [0,1]
                if(currentDepth - bias > closestDepth)
                    shadow += 0.0;
                else
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

	return shadow;
}

void main() {
    vec4 materialColor = texture(DiffuseTexture, UV);//材料纹理颜色
    float alpha = materialColor.a;//材料透明度
    float occlusion = 0.0;
    float specularOcclusion = 0.0;
    if(alpha < 0.5) {
        discard;
    }//不渲染透明物体

    //TBN矩阵
    TBN = inverse(transpose(mat3(WorldTangent, WorldBitangent, WorldNormal)));

    vec3 N = calcBumpNormal();//从高度贴图计算法向量
    vec3 L = normalize(LightPos - WorldPosition); //光源方向
    vec3 E = normalize(WorldLookAtDir);//相机相对方向
   
    // 计算可见度
    float visibility = countShadow(0.002);

//计算漫反射光
    vec3 diffuseReflection;
    {

        // 直接漫反射
        float cosTheta = max(0, dot(N, L));//入射光线和法线余弦值
        vec3 directDiffuseLight = vec3(visibility * cosTheta);

        //间接漫反射
       vec4 indirectDiffuseLight = vec4(0.f);
        for(int i = 0; i < NUM_CONES; i++) //叠加六个锥体
        {
            indirectDiffuseLight += coneWeights[i] * coneTrace(normalize(TBN * coneDirections[i]), 0.577);
        }
        occlusion = 1 - indirectDiffuseLight.a;
        indirectDiffuseLight = indirectDiffuseLight;

        // 漫反射求和
        diffuseReflection =  (directDiffuseLight + occlusion * indirectDiffuseLight.rgb ) * materialColor.rgb;
    }
    
    // 计算镜面反射光
    vec3 specularReflection;
    {
        vec4 specularColor = texture(SpecularTexture, UV);
        specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;

        //直接镜面反射
        vec3 reflectDir = normalize(reflect(-L, N));  
        float spec = pow(max(dot(E, reflectDir), 0.0), Shininess);
        vec3 directSpecularLight = vec3 (spec * visibility); 
        directSpecularLight = directSpecularLight;
       
       //间接镜面反射
        vec3 inlightDir = normalize(reflect(-E,N));
        vec4 tracedSpecular = coneTrace(inlightDir, tan(0.3)); //  0.07 = 8 degrees angle
        tracedSpecular = tracedSpecular;
        specularOcclusion = 1 - tracedSpecular.a;
        //镜面反射求和
        specularReflection =   (tracedSpecular.rgb+ specularOcclusion * directSpecularLight) * specularColor.rgb ;
       
    }
    //环境光
    vec3 ambientLight = ambientFactor * materialColor.rgb * occlusion;
    //光总量求和
    frag_color = vec4(ambientLight + diffuseReflection + specularReflection, alpha);

}