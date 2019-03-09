#version 330

#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float4x4 mat4
#define float3x3 mat3

in float2 fragmentTexCoord;

layout(location = 0) out vec4 fragColor;
uniform int g_screenWidth;
uniform int g_screenHeight;

struct Material {
  float4 Ka;
  float4 Kd;
  float4 Ks;
  float Shine;
};

struct Light {
  float3 Coord;
  float4 Color;
};

struct Box {
  float3 Sizes;
  float3 Center;
  float3 Color;
  Material Mat;
};

struct Shere {
  float3 Center;
  float Rad;
  float3 Color;
  Material Mat;
};

struct Ground {
  float4 Coord;
  float3 Color;
  Material Mat;
};

struct CSG {
  float3 CenterS;
  float Rad;
  float3 Sizes;
  float3 CenterP;
  Material Mat;
};

const int MaxSteps = 255*10;
const float MinDist = 0.001;
const float MaxDist = 100.0;
const float Epsilon = 0.00001;

Shere Shere1;
Shere Shere2;
Box Box1;
Box Box2;
Ground Ground1;
Light Light1;
Light Light2;
CSG CSG1;
void Set(){
  Shere1.Center = float3(3, 3, 0.5); 
  Shere1.Rad = 0.5; 
  Shere1.Color = float3(0, 0, 1.0);
  Shere1.Mat.Ka = float4(0.25, 0.148, 0.06475, 1);    // Polished Bronze
  Shere1.Mat.Kd = float4(0.4, 0.2368, 0.1036, 1);
  Shere1.Mat.Ks = float4(0.774597, 0.458561, 0.200621, 1);
  Shere1.Mat.Shine = 76.8;

  Shere2.Center = float3(5, 5, 2); 
  Shere2.Rad = 1.0; 
  Shere2.Color = float3(0, 1.0, 0.0);
  Shere2.Mat.Ka = float4(0.25, 0.20725, 0.20725, 0.922);        //  Pearl
  Shere2.Mat.Kd = float4(1, 0.829, 0.829, 0.922);
  Shere2.Mat.Ks = float4(0.296648, 0.296648, 0.296648, 0.922);
  Shere2.Mat.Shine = 11.264 ;

  Box1.Sizes = float3(1.5, 1.5, 1.5);
  Box1.Center = float3(-0.95, 0.01, 0.2);
  Box1.Color = float3(0.5, 0, 0.13);
  Box1.Mat.Ka = float4(0.23125, 0.23125, 0.23125, 1);          //   Polished Silver
  Box1.Mat.Kd = float4(0.2775, 0.2775, 0.2775, 1); 
  Box1.Mat.Ks = float4(0.773911, 0.773911, 0.773911, 1);
  Box1.Mat.Shine = 89.6;

  Box2.Sizes = float3(0.5, 1, 1);
  Box2.Center = float3(1, 0, 0.5);
  Box2.Color = float3(204.0/255, 119.0/255, 34.0/255);
  Box2.Mat.Ka = float4(0.0215, 0.1745, 0.0215, 0.55 );            //    Emerald
  Box2.Mat.Kd = float4(0.07568, 0.61424, 0.07568, 0.55 );
  Box2.Mat.Ks = float4(0.633, 0.727811, 0.633, 0.55 );
  Box2.Mat.Shine = 76.8;

  Ground1.Coord = normalize(float4(0, 0, 1, 1));    
  Ground1.Color = float3(0, 0.5, 0.5);
  Ground1.Mat.Ka = float4(0.135, 0.2225, 0.1575, 0.95);            //Jade
  Ground1.Mat.Kd = float4(0.54, 0.89, 0.63, 0.95 );
  Ground1.Mat.Ks = float4(0.316228, 0.316228, 0.316228, 0.95 );
  Ground1.Mat.Shine = 12.8;

  CSG1.CenterS = float3(10, 10, 0.5); 
  CSG1.Rad = 1.5;
  CSG1.Sizes = float3(3.5, 1.5, 2.0)/2;
  CSG1.CenterP = float3(10, 10, 0.5);
  CSG1.Mat.Ka = float4(0.05375, 0.05, 0.06625, 0.82);                 //Obsidian  
  CSG1.Mat.Kd = float4(0.18275, 0.17, 0.22525, 0.82 );
  CSG1.Mat.Ks = float4(0.332741, 0.328634, 0.346435, 0.82);
  CSG1.Mat.Shine = 38.4; 

  Light1.Coord = float3(5, 5, 10);
  Light1.Color = float4(1, 1, 1, 1);

  Light2.Coord = float3(-5, -15, 7);
  Light2.Color = float4(1, 1, 1, 1);
};

uniform float4x4 g_rayMatrix;

uniform float4   g_bgColor = float4(0,0,1,1);

float Kc = 0.3;
float Kl = 0.1;
float Kq = 0.005;

float3 EyeRayDir(float x, float y, float w, float h) {
	float fov = 3.141592654f/(2.0f); 
  float3 ray_dir;
	ray_dir.x = x+0.5f - (w/2.0f);
	ray_dir.y = y+0.5f - (h/2.0f);
	ray_dir.z = -(w)/tan(fov/2.0f);
  return normalize(ray_dir);
}

float BoxDist(float3 pos, float3 b, float3 BoxCenter) {
  return length(max(abs(pos-BoxCenter)-b,0.0));
}

float ShereDist(float3 pos, float ShereRad, float3 ShereCenter) {
  return length(pos-ShereCenter)-ShereRad;
}

float CSGDist(float Sh, float P) {
  return max(-Sh, P);
}

float GroundDist(float3 pos, float4 n)
{
  // n must be normalized
  return dot(pos,n.xyz) + n.w;
}

float minDist1(float3 p) {
  float m1 = min(BoxDist(p, Box1.Sizes, Box1.Center), BoxDist(p, Box2.Sizes, Box2.Center));
  float m2 = min(ShereDist(p, Shere1.Rad, Shere1.Center), ShereDist(p, Shere2.Rad, Shere2.Center));
  float m3 = min(m1, m2);
  float Sh = ShereDist(p, CSG1.Rad, CSG1.CenterS);
  float P = BoxDist(p, CSG1.Sizes, CSG1.CenterP);
  float m4 = CSGDist(Sh, P);
  float m5 = GroundDist(p, Ground1.Coord);
  return min(min(m3, m4), m5);
}

float minDist(float3 ray_pos, float3 ray_dir, float depth) {
  return minDist1(ray_pos + depth * ray_dir);
}

int Kind(float dist, float3 ray_pos, float3 ray_dir) {
    if (dist == MaxDist)
      return -1;
    else if (BoxDist(ray_pos + dist * ray_dir, Box1.Sizes, Box1.Center) == minDist(ray_pos, ray_dir, dist))
      return 0;
    else if (BoxDist(ray_pos + dist * ray_dir, Box2.Sizes, Box2.Center) == minDist(ray_pos, ray_dir, dist))
      return 1;
    else if (ShereDist(ray_pos + dist * ray_dir, Shere1.Rad, Shere1.Center) == minDist(ray_pos, ray_dir, dist))
      return 2;
    else if (ShereDist(ray_pos + dist * ray_dir, Shere2.Rad, Shere2.Center) == minDist(ray_pos, ray_dir, dist))
      return 3;
    else if (GroundDist(ray_pos + dist * ray_dir, Ground1.Coord) == minDist(ray_pos, ray_dir, dist))
      return 4;
    else 
      return 5; //CSG
}

float InterDist(float3 ray_pos, float3 ray_dir) {   
    float depth = MinDist;                         
    for (int i = 0; i < MaxSteps; i++) {  
        float dist =  minDist(ray_pos, ray_dir, depth);
        if (dist < Epsilon) {
			    return depth+dist;
        }
        depth += dist;
        if (depth >= MaxDist) {
            return MaxDist;
        }
    }
    return MaxDist;
}

float3 EstimateNormal(float3 z, float eps) {
  float3 z1 = z + float3(eps, 0, 0);
  float3 z2 = z - float3(eps, 0, 0);
  float3 z3 = z + float3(0, eps, 0);
  float3 z4 = z - float3(0, eps, 0);
  float3 z5 = z + float3(0, 0, eps);
  float3 z6 = z - float3(0, 0, eps);
  float dx = minDist(z1, float3(0, 0, 0), 0) - minDist(z2, float3(0, 0, 0), 0);
  float dy = minDist(z3, float3(0, 0, 0), 0) - minDist(z4, float3(0, 0, 0), 0);
  float dz = minDist(z5, float3(0, 0, 0), 0) - minDist(z6, float3(0, 0, 0), 0);
  return normalize(float3(dx, dy, dz) / (2.0*eps));
}

float AmbientOcclusion(float3 pos, float3 n) {
  const int fluent = 3;
  int check[fluent];
  float Rad = 0.025;
  float num = 50;
  float angle = 0;
  float3 curr;
  float3 k1;
  if (dot(n, float3(0, 0, 1)) == 1)
    k1 = normalize(cross(n, float3(1, 1, 0)));
  else 
    k1 = normalize(cross(n, float3(0, 1, 1)));
  float3 k2 = normalize(cross(n, k1));
  for (int j = 0; j < fluent; j++) {
    check[j]=0;
    for (int i = 0; i < num; i++) {
      curr = Rad * normalize(float3(pos + k1*cos(3.14*i/num) + k2*sin(3.14*i/num) + n));
      if  (minDist1(pos + curr) < Rad*cos(3.14*i/num)*sin(3.14*i/num))
        check[j] ++;
    }
    Rad *= 2;
  }
  float shadow=0;
  for (int i = 0; i < fluent; i++)
    shadow += pow(check[i]/num, i+1);
  return 1.0-shadow;
}

float4 Shade(float3 light_pos, float4 color, float3 pos, float3 obs_pos, Material Mat) {    
  float dist = InterDist(light_pos, normalize(pos-light_pos));
  float att = 1.0/(Kc+Kl*dist*(1+Epsilon)+0.05*Kq*dist*dist*(1+Epsilon)*(1+Epsilon));
  float3 n = EstimateNormal(pos, Epsilon);
  if (dist*(1+Epsilon) < length(light_pos-pos))  // Shadow
    return Mat.Ka*AmbientOcclusion(pos, n);
  else {
    float Diffuse = dot(n, normalize(light_pos-pos));
    float3 Reflect = normalize(reflect(-light_pos+pos, n));
    float Specular = dot(n, normalize(obs_pos-pos));
    Specular = pow(Specular, Mat.Shine);
    return Mat.Ka*AmbientOcclusion(pos, n) + color*(Mat.Kd*Diffuse*att+ Mat.Ks*Specular*att);
  }
}

float4 RayMarch(float3 ray_pos, float3 ray_dir) {
	float4 color = float4(0,0.5,1, 2)/2;  // sky
  float InDist = InterDist(ray_pos, ray_dir);
  float4 I1, I2;
  float4 I;
  int kind = Kind(InDist, ray_pos, ray_dir);
  switch(kind) {
    case 0:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, Box1.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, Box1.Mat);
      I = I1+I2;
      return I;
      break;
    case 1:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, Box2.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, Box2.Mat);
      I = I1+I2;
      return I;
      break;
    case 2:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, Shere1.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, Shere1.Mat);
      I = I1+I2;
      return I;
      break;
    case 3:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, Shere2.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, Shere2.Mat);
     I = I1+I2;
      return I;
      break;
    case 4:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, Ground1.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, Ground1.Mat);
      I = I1+I2;
      return I;
      break;
    case 5:
      I1 = Shade(Light1.Coord, Light1.Color, ray_pos + InDist*ray_dir, ray_pos, CSG1.Mat);
      I2 = Shade(Light2.Coord, Light2.Color, ray_pos + InDist*ray_dir, ray_pos, CSG1.Mat);
      I = I1+I2;
      return I;
      break;
  }
	return color;
}


void main(void) {	
  Set();
  float w = float(g_screenWidth);
  float h = float(g_screenHeight);
  // get curr pixelcoordinates
  //
  float x = fragmentTexCoord.x*w; 
  float y = fragmentTexCoord.y*h;
  // generate initial ray
  //
  float3 ray_pos = float3(5,5,5); 
  float3 ray_dir = EyeRayDir(x,y,w,h);
  // transorm ray with matrix
  //
  ray_pos = (g_rayMatrix*float4(ray_pos,1)).xyz;
  ray_dir = normalize(float3x3(g_rayMatrix)*ray_dir);
  // intersect bounding box of the whole scene, if no intersection found return background color
  // 
	float alpha = 1.0f;
	float4 color = RayMarch(ray_pos, ray_dir);
	fragColor = color;
}