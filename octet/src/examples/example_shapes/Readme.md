---
title:The Cube
description: Puzzle Level Game
author: Eleni Paskali
tags: master, project octet framework
created: 2015 Oct 15
modified: 2015 Nov 15
---
The Cube
---------
---------
##Puzzle Level Game

The Cube is a puzzle level game. The purpose is to throw all the cubes before you run out of time. 
There are invisible obstacles where you cannot access the cubes in order to throw them, you have to find 
a different path or gain force to attack on them.
The yellow cubes are the enemies. Try not to put them in the corner of the maze, otherwise you won't be able to finish the Level.

The Cube is created in C++ Octet Game Framework. 

To play the game you need to have Visual Studio 2013 for Windows Desktop.

 Simply run:

example_shapes -> example_shapes.vcxproj

To start the Level 1 press the button 1
To continue to the next Levels press button 2 and 3.You can choose any Level to play by  pressing the buttons 1, 2 and 3 accordingly.

All levels have benn created according the txt files.

Level 1 contains the enemies and the player. The player can go all around the maze , however the enemies do not move but there are invisible restrictions to gain access to them. The enemies are being represented by the letter "E" from a txt file. The level is being read from Level1.txt 
 where the maze is being created from cubes.The wall of the maze is represented with the letter "W". The player is being represented with the letter "C" which is the character in the txt file, the blue box.

Level 2 is being read from Level2.txt file , but there are more restrictions and invisible obsatcles and cubes to throw in abyss in a more limited time. The same mentality is for the creation of Level 3. As you grow Levels the difficulty becomes bigger.

The file4.txt conatains the welcoming of the player. It is saying "HELLO" and press 1 to proceed to the game. All the letters have been writen /created by cubes, trying to represent a taste of Minecraft world. 

You can navigate through the levels and play each level you would like.There are no restrictions for the moment that you need to complete each level to procced to the next one. The maze is the main creation of the game, where i had to calculate the x, y, and z position of the player during the navigation in the maze. 

I have calculated the x, y and z position through spring constraits setting the limits 0 for x axis, 1 to 3 from y axis and 2 to 3 for z axis :

springConstraint->setLimit(0, 0, 0); //X Axis
springConstraint->setLimit(1, 3, -3); //Y Axis
springConstraint->setLimit(2, 3, -3); //Z Axis

The motion of the player has the same like the enemy with the difference that the player can move and the enemy cannot. There is a future aspect to create a random movement of the enemy trying the player to catch it and throw it in the abyss.
The interaction between the enmy and the player has been succeded by creating rigid bodies setting the linear(1, 1, 0) and angular factor (0, 0, 1).

The invisible restrictions can be called as stopers and are being represented from the txt file by the letter "S".

After the calculation of the positions  x, y and z axis  i can get the local identity of the stoppers and the player.

Not all the rigid bodies i have created have been imported in the main game but there are to be in the future as i want to create a big game with many levels. 

The background has been created from a box containing text in pixels and creating an overlay(by adding a mesh) with a default font, where i am calling  open gl functions :(glClearColor(0.7, 0.5, 0.0, 0.0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 The other half of the background is a box again by creating add_shape and setting the size and the color of our choice, in this case i choose the color green by creating a material *green with RGB colors (0, 1, 0, 1).

Each level has different color and being created from setting the material as mentioned above.

The main play is a navigation through keyboard by setting the key inputs with left, right, up and down arrows. However to gain force and jump through obstacles i set the key space.

The music bacground is being read from a file and imported from the function "Playsound":
Playsound("Ghost.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_LOOP);
The sound is in loop and does not wait until the playback is over.
 
The idea of the game has been created according the example_shapes of the Octet Game Framework.
There is a construction of Physics demo showing :

Hinge constraints
Spring constraints
Collision callbacks
Setting up objects by reading text file.


***You can see below a demo of the work:***

youtube video:
https://www.youtube.com/watch?v=RCMsIyrOjuU&feature=youtu.be

***Screenshots of the game:***

https://dl.dropboxusercontent.com/Camera%20Uploads/screenshoots/Screenshot+(101).png?raw=true

https://raw.githubusercontent.com/elenaamanita/octet/26199013addc60c3945641cf9518ffce2a7a10dd/octet/src/examples/example_shapes/Hello.png

https://github.com/elenaamanita/octet/blob/26199013addc60c3945641cf9518ffce2a7a10dd/octet/src/examples/example_shapes/Level1.png

https://github.com/elenaamanita/octet/blob/26199013addc60c3945641cf9518ffce2a7a10dd/octet/src/examples/example_shapes/Level2.png

https://github.com/elenaamanita/octet/blob/26199013addc60c3945641cf9518ffce2a7a10dd/octet/src/examples/example_shapes/Level3.png

