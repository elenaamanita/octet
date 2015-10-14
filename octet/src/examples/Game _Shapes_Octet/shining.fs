varying vec3 normal_;
varying vec3 camera_pos_;
varying vec2 uv_;
varying vec4 color_;
varying vec3 model_pos_;
varying vec3 position;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;
uniform float shnn;

uniform vec3 l_ambient;
uniform vec3 l_diffuse;
uniform vec3 l_specular;

uniform vec3 lightpos;

void main()
{

vec3 ambient = m_ambient*l_ambient;

vec3 surfacetolight = normalize(lightpos - position);
vec3 norm = normalize(normal_);
float d_cont = dot(norm,surfacetolight);
if(d_cont <0.0)
{d_cont=0.0;}
vec3 diffuse = d_cont*m_diffuse * l_diffuse;

vec3 surfacetoview = normalize(camera_pos_ - position);
vec3 reflection = reflect(-surfacetolight,norm);
float s_cont = dot(surfacetoview,reflection);

if(s_cont<0.0)
{s_cont=0.0;}
float s_contp = pow(s_cont,shnn);
vec3 specular = s_contp*l_specular*m_specular;


gl_FragColor = vec4(ambient+diffuse+specular, 1.0);
//  gl_FragColor = vec4(m_ambient, 1.0);
}