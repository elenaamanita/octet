////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>

namespace octet {
  /// Scene containing a box with octet.
  class tandm_game : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	random r;

	string contents;
	char charArray[3][10];

	int levelNumber = 1;

	mesh_box *box;
	mesh_sphere *sph;
	material *mat;
  public:
    /// this is called when we construct the class before everything is initialised.
    tandm_game(int argc, char **argv) : app(argc, argv) {
    }

	//clear the scene
	void newScene()
	{
		app_scene->reset();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->set_far_plane(1000);
		scene_node *cam = app_scene->get_camera_instance(0)->get_node();
		cam->translate(vec3(0, 0, 100));
	}

	//read txt file and get level data
	void loadTxt()
	{
		std::fstream myFile;
		std::stringstream fileName;
		fileName << "level" << levelNumber << ".txt";
		myFile.open(fileName.str().c_str(), std::ios::in);
		if (!myFile.is_open())
		{
			printf("File not opened/is missing\n");
		}

		else
		{
			printf("File open\n");
			myFile.seekg(0, myFile.end);
			int length = myFile.tellg();
			myFile.seekg(0, myFile.beg);

			std::stringstream file;
			file.str(std::string());
			file << myFile.rdbuf();
			myFile.close();

			contents = file.str().c_str();
			printf("%s\n", contents.c_str());
			createLevel();
		}
	}

	void createLevel()
	{
		vec3 pos = vec3(0, 0, 0);
		int x = 0;
		for (int i = 0; i < contents.size(); i++)
		{
			scene_node *node = new scene_node;
			char c = contents[i];
			switch (c)
			{
			case '\n': pos -= (vec3(x, 0, 0));
				x = 0;
				pos += vec3(0, -1, 0);
				break;
			case '-': 
			case '/': 
			case 'P': 
				x += 1; 
				pos += vec3(1, 0, 0);
				break;
			case 'B': node->translate(pos);
				app_scene->add_child(node);
				app_scene->add_mesh_instance(new mesh_instance(node, box, mat));
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case '_': node->translate(pos);
				app_scene->add_child(node);
				app_scene->add_mesh_instance(new mesh_instance(node, box, mat));
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			default : break;
			}
		}
	}

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
	  box = new mesh_box(0.5f);
	  sph = new mesh_sphere(vec3(0, 0, 0), 1, 1);
	  mat = new material(vec4(1, 0, 0, 1));
	  newScene();
	  loadTxt();
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
