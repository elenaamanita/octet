////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class example_shapes : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;

	///Bullet Physics
	///set up world
	btDefaultCollisionConfiguration config;
	///handle collisions
	btCollisionDispatcher *dispatcher;
	///handle collisions
	btDbvtBroadphase *broadphase;
	///resolve collisions
	btSequentialImpulseConstraintSolver *solver;
	///physics world
	btDiscreteDynamicsWorld *world;
	///set the shader
	param_shader *b;
	///material for the shader
	ref<param_uniform> m_amb;
	///light for the shader
	ref<param_uniform> l_amb;
	///material diffuse for the shader
	ref<param_uniform> m_dif;
	///light diffuse for the shader
	ref<param_uniform> l_dif;
	///the material specular for the shader
	ref<param_uniform> m_spec;
	/// light specular for the shader
	ref<param_uniform> l_spec;
	///shining for the  shader
	ref<param_uniform> shnn;
	/// light position for the shader
	ref<param_uniform> lightpos;
	///the material for the shader
	ref<material> shiningbox;

	//////parameters values for the shader
	float shnnval = 32;

	vec3 amb;

	vec3 lamb;

	vec3 diff;

	vec3 ldiff;

	vec3 spec;

	vec3 lspec;

	vec3 light_pos;

	void init_shiningbox(){

		b = new param_shader("src/examples/example_shapes/base_vertex.vs", "src/examples/example_shapes/shining.fs");
		shiningbox = new material(vec4(1.0f, 1.0f, 1.0f, 1.0f), b);
		atom_t atom_shinn = app_utils::get_atom("shnn");
		shnn = shiningbox->add_uniform(&shnnval, atom_shinn, GL_FLOAT, 1, param::stage_fragment);
		atom_t atom_m_amb = app_utils::get_atom("m_ambient");
		amb = vec3(0.4f, 0.4f, 0.4f);
		m_amb = shiningbox->add_uniform(&amb, atom_m_amb, GL_FLOAT_VEC3, 1, param::stage_fragment);
		atom_t atom_l_amb = app_utils::get_atom("l_ambient");
		lamb = vec3(0.55f, 0.55f, 0.55f);
		l_amb = shiningbox->add_uniform(&lamb, atom_l_amb, GL_FLOAT_VEC3, 1, param::stage_fragment);

		atom_t atom_m_diff = app_utils::get_atom("m_diffuse");
		diff = vec3(0.6f, 0.6f, 0.6f);
		m_dif = shiningbox->add_uniform(&diff, atom_m_diff, GL_FLOAT_VEC3, 1, param::stage_fragment);

		atom_t atom_l_diff = app_utils::get_atom("l_diffuse");
		ldiff = vec3(0.6f, 0.6f, 0.6f);
		l_dif = shiningbox->add_uniform(&ldiff, atom_l_diff, GL_FLOAT_VEC3, 1, param::stage_fragment);

		atom_t atom_m_spec = app_utils::get_atom("m_specular");
		spec = vec3(0.9f, 0.9f, 0.9f);
		m_spec = shiningbox->add_uniform(&spec, atom_m_spec, GL_FLOAT_VEC3, 1, param::stage_fragment);

		atom_t atom_l_spec = app_utils::get_atom("l_specular");
		lspec = vec3(1.0f, 1.0f, 1.0f);
		l_spec = shiningbox->add_uniform(&lspec, atom_l_spec, GL_FLOAT_VEC3, 1, param::stage_fragment);

		atom_t atom_lightpos = app_utils::get_atom("lightpos");
		light_pos = vec3(app_scene->get_light_instance(0)->get_node()->access_nodeToParent().w().x(), app_scene->get_light_instance(0)->get_node()->access_nodeToParent().w().y(), app_scene->get_light_instance(0)->get_node()->access_nodeToParent().w().z());
		lightpos = shiningbox->add_uniform(&light_pos, atom_lightpos, GL_FLOAT_VEC3, 1, param::stage_fragment);
	}

	enum Directions
	{
		UP,
		DOWN,
		LEFT,
		RIGHT

	};
  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
		delete world;
		delete solver;
		delete broadphase;
		delete dispatcher;
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
	  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
	  app_scene->get_camera_instance(0)->set_far_plane(5500);
	  app_scene->get_light_instance(0)->get_node()->access_nodeToParent().rotateY(90);

	  init_shiningbox();

      //material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
      material *blue = new material(vec4(0, 0, 1, 1));

      mat4t mat;
    ///  mat.translate(-3, 6, 0);
    ///  app_scene->add_shape(mat, new mesh_sphere(vec3(2, 2, 2), 2), red, true);

	  ///import boxes
      mat.loadIdentity();
      mat.translate(0, 10, 0);
      //app_scene->add_shape(mat, new mesh_box(vec3(1, 1, 1)), blue, true);
	  ///add more boxes in the scene
	  for (size_t i = 0; i < 10; i++)
	  {
		  app_scene->add_shape(mat, new mesh_box(vec3(1, 1, 1)), green, true);
	  }
	  


      ///mat.loadIdentity();
     /// mat.translate( 3, 6, 0);
     /// app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, true);

      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), blue, false);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}
