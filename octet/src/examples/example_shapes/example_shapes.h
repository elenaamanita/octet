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
	class example_shapes : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;

		ref<camera_instance> camera;

		// Needed for the physics simulation
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

		mat4t worldCall;

		btRigidBody *staticObject;

		dynarray<btRigidBody*> rigid_bodies;
		dynarray<btHingeConstraint*> bullets;

		dynarray<btGeneric6DofConstraint*> springBodies;

		mesh_box *box, *bulletMesh, *blockerMesh;
		mesh_sphere *sph;
		material *wall, *floor, *turn, *end, *enemy, *character;

		scene_node *cam;

		///character_id
		int character_node;

		///enemy_id
		int enemy_node;

		///hinge variables
		bool hingeOffsetNotSet = false;
		btVector3 join;


		string contents;


	public:
		///this is called when we construct the class before everything is initialised.
		example_shapes(int argc, char **argv) : app(argc, argv) {

			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}
		///clear
		~example_shapes() {

			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}



		///create a mesh with Rigid Body
		void add_part(mat4t_in coord, mesh *shape, material *mat, bool is_dynamic, char letter)
		{
			scene_node *node = new scene_node();
			node->access_nodeToParent() = coord;
			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, shape, mat));

			btMatrix3x3 matrix(get_btMatrix3x3(coord));
			btVector3 pos(get_btVector3(coord[3].xyz()));

			btCollisionShape *collisionShape = shape->get_bullet_shape();
			if (collisionShape)
			{
				btTransform transform(matrix, pos);
				btDefaultMotionState *motionState = new btDefaultMotionState(transform);
				btScalar mass = is_dynamic ? 0.5f : 0.0f;
				btVector3 inertiaTensor;

				collisionShape->calculateLocalInertia(mass, inertiaTensor);

				btRigidBody *rigid_body = new btRigidBody(mass, motionState, collisionShape, inertiaTensor);
				world->addRigidBody(rigid_body);
				rigid_bodies.push_back(rigid_body);
				rigid_body->setUserPointer(node);
			}
		}

		//call to create mesh and set position with friction and restitution
		void add_rigid_body(vec3 position, mesh *msh, material *mat, char letter, bool active)
		{
			worldCall.translate(position);
			add_part(worldCall, msh, mat, active, letter);

			rigid_bodies.back()->setFriction(0);
			rigid_bodies.back()->setRestitution(0);
			///worldCall.loadIdentity();
			///if (letter == 'B' || letter == 'F')
			///{
			///	btHingeConstraint *bullet = new btHingeConstraint((*rigid_bodies.back()), btVector3(0.25f*0.95f, 0, 0.125f*-1.2f), btVector3(0, 0, 1.0f), false);
			///bullet->setLimit(-3.14f*0.2f, -3.14f*0.2f);
			///bullets.push_back(bullet);
			///	world->addConstraint(bullet);
			///}
			///}
			//static object
			if (letter == 'B')
			{
				staticObject = rigid_bodies.back();
			}

			///character object
			if (letter == 'C')
			{
				//gets the character node
				character_node = rigid_bodies.size() - 1;
				//constraints z axis movement
				rigid_bodies.back()->setLinearFactor(btVector3(1, 1, 0));
				//constraints x and y axis rotation
				rigid_bodies.back()->setAngularFactor(btVector3(0, 0, 1));
			}

			///enemy object
			if (letter == 'E')
			{
				//gets the enemy node
				enemy_node = rigid_bodies.size() - 1;
				//constraints z axis movement
				rigid_bodies.back()->setLinearFactor(btVector3(1, 1, 0));
				//constraints x and y axis rotation
				rigid_bodies.back()->setAngularFactor(btVector3(0, 0, 1));
			}
			///spring stoppers
			if (letter == 'S')
			{
				btTransform localA, localB;
				localA.setIdentity();
				localB.setIdentity();
				localA.getOrigin() = btVector3(position.x(), position.y(), position.z());
				btRigidBody *springBody = rigid_bodies.back();
				springBody->setLinearFactor(btVector3(0, 1, 0));
				btGeneric6DofSpringConstraint *springConstraint = new btGeneric6DofSpringConstraint(*staticObject, *springBody, localA, localB, true);
				springConstraint->setLimit(0, 0, 0); //X Axis
				springConstraint->setLimit(1, 3, -3); //Y Axis
				springConstraint->setLimit(2, 3, -3); //Z Axis
				springConstraint->setLimit(3, 0, 0);
				springConstraint->setLimit(4, 0, 0);
				springConstraint->setLimit(5, 0, 0);
				springConstraint->enableSpring(1, true); //int index implies the axis you want to move in
				springConstraint->setStiffness(1, 100);
				world->addConstraint(springConstraint);
				rigid_bodies.back()->applyCentralForce(btVector3(0, 100, 0));
				springBodies.push_back(springConstraint);
			}
			///hinge
			if (letter == 'H')
			{
				if (!hingeOffsetNotSet)
				{
					join = get_btVector3(bulletMesh->get_aabb().get_max());
					join = btVector3(join.x()*0.5f, join.y()*0.0f, join.z()*0.0f);
					printf("%g %g %g\n", join.x(), join.y(), join.z());
					hingeOffsetNotSet = true;
				}
				btRigidBody *hingeBody = rigid_bodies.back();
				btHingeConstraint *bulletHinge = new btHingeConstraint(*hingeBody, join, btVector3(0, 0, 1));
				world->addConstraint(bulletHinge);
				bullets.push_back(bulletHinge);
			}

			worldCall.loadIdentity();
		}
		//clear the scene
		void newScene()
		{

			rigid_bodies.reset();
			bullets.reset();

			app_scene->reset();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->set_far_plane(1000);
			cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(24, -24, 50));
			box = new mesh_box(0.5f);
			bulletMesh = new mesh_box(vec3(0.5f, 0.25f, 0.5f));
			blockerMesh = new mesh_box(vec3(0.5f, 1, 0.5f));
			sph = new mesh_sphere(vec3(0, 0, 0), 1, 1);
			wall = new material(vec4(1, 0, 0, 1));
			floor = new material(vec4(0, 1, 0, 1));
			turn = new material(vec4(0, 0, 1, 1));
			end = new material(vec4(1, 1, 1, 1));
			character = new material(vec4(0, 1, 1, 0));
			enemy = new material(vec4(1, 1, 0, 0));


			join = btVector3(0, 0, 0);
		}

		//read txt file and get level data
		void loadTxt(int num)
		{
			std::fstream myFile;
			std::stringstream fileName;
			fileName << "level" << num << ".txt";
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
				//printf("%s\n", contents.c_str());
				createLevel();
			}
		}

		//create the level according to the txt file
		void createLevel()
		{
			vec3 pos = vec3(0, 0, 0);
			int x = 0;
			for (int i = 0; i < contents.size(); i++)
			{
				char c = contents[i];
				switch (c)
				{
				case '\n': pos -= (vec3(x, 0, 0));
					x = 0;
					pos += vec3(0, -1, 0);
					break;
				case ' ':
				case '/':
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case '_': add_rigid_body(pos, box, floor, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;


					/*case 'B':
					case 'F': add_rigid_body(pos, bulletMesh, flip, c, true);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
					case 'E': add_rigid_body(pos, box, end, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
					case 'P': add_rigid_body(pos, box, player, c, true);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
					case '-':
					case '¦': add_rigid_body(pos, box, wall, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
					default: break;
					}
					}
					}
					*/

				case 'W':add_rigid_body(pos, box, wall, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case 'B':
				case 'A': add_rigid_body(pos, bulletMesh, turn, c, true);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case 'D': add_rigid_body(pos, box, end, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case 'C': add_rigid_body(pos, box, character, c, true);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case 'E': add_rigid_body(pos, box, enemy, c, true);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				case '-':
				case '¦': add_rigid_body(pos, box, wall, c, false);
					x += 1;
					pos += vec3(1, 0, 0);
					break;
				default: break;
				}
			}
		}

		/// this is called once OpenGL is initialized
		void app_init() {

			app_scene = new visual_scene();

			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
			
			newScene();
			loadTxt(4);

			///material *background = new material(new image("example_shapes/background.gif"), NULL, atten_shader, true);
			material *green = new material(vec4(0, 1, 0, 1));

			mat4t mat;
			

			// ground
			mat.loadIdentity();
			mat.translate(0, -1, 0);
			app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			world->stepSimulation(1.0f / 30, 1.0f / 30, 1.0f / 30);
			btCollisionObjectArray &colArray = world->getCollisionObjectArray();
			for (unsigned i = 0; i != colArray.size(); ++i)
			{
				btCollisionObject *colObj = colArray[i];
				scene_node *node = (scene_node *)colObj->getUserPointer();
				if (node)
				{
					mat4t &modelToWorld = node->access_nodeToParent();
					colObj->getWorldTransform().getOpenGLMatrix(modelToWorld.get());
				}
			}

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);


			if (is_key_going_down('1') || is_key_going_down(VK_NUMPAD1))
			{
				newScene();
				loadTxt(1);

				material *lightblue = new material(vec4(0, 1, 1, 1));

				mat4t mat;
				
				// ground
				mat.loadIdentity();
				mat.translate(0, -1, 0);
				app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), lightblue, false);
			}
			if (is_key_going_down('2') || is_key_going_down(VK_NUMPAD2))
			{
				newScene();
				loadTxt(2);

				material *lightyellow = new material(vec4(1, 1, 0, 0));

				mat4t mat;
				

				// ground
				mat.loadIdentity();
				mat.translate(0, -1, 0);
				app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), lightyellow, false);
			}

			if (is_key_going_down('3') || is_key_going_down(VK_NUMPAD3))
			{
				newScene();
				loadTxt(3);

				material *magenta = new material(vec4(1, 0, 0, 0));

				mat4t mat;
				

				// ground
				mat.loadIdentity();
				mat.translate(0, -1, 0);
				app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), magenta, false);
			}
			//KEY INPUTS
			if (is_key_down(VK_SPACE))
			{
				rigid_bodies[character_node]->applyCentralForce(btVector3(0, 25, 0));
			}

			else if (is_key_down(key_right))
			{
				rigid_bodies[character_node]->applyCentralForce(btVector3(15, 0, 0));
			}

			else if (is_key_down(key_left))
			{
				rigid_bodies[character_node]->applyCentralForce(btVector3(-15, 0, 0));
			}
			else if (is_key_down(key_down))
			{
				rigid_bodies[character_node]->applyCentralForce(btVector3(0, -15, 0));
			}

			else if (is_key_down(key_up))
			{
				rigid_bodies[character_node]->applyCentralForce(btVector3(0, 15, 0));
			}

			

			/*//bullet test
			if (is_key_going_down('F'))
			{
			for (int i = 0; i < bullets.size(); i++)
			{
			bullets[i]->getRigidBodyA().applyTorqueImpulse(btVector3(0, 0, 500));
			}
			*/
		}
	};
}
	



