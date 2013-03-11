#include <iostream>
#include <string.h>
//#include "VocalSynthesizerSpeaker.h"
#include "AbstractSpeaker.h"
#include "ConsoleSpeaker.h"
#include <SDL/SDL.h>


using namespace std;







int main()
{
    AbstractSpeaker* speaker = new ConsoleSpeaker();

    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0)
    {
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }


    cout << "There is (are) " << SDL_NumJoysticks() << " josticks";
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(320, 240, 0, SDL_ANYFORMAT);
    if (screen == NULL) {
        cout << "Unable to set video mode: " << SDL_GetError() << endl;
        return 1;
    }

    Uint8 *keys = SDL_GetKeyState(NULL);
    SDL_Event event;

    SDL_Joystick *joystick; // attention, c'est bien un pointeur !
    joystick = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);


 // Main loop
   while(1)
   {

      // Check for messages
      if (SDL_PollEvent(&event))
      {

          switch(event.type)
            {
                case SDL_JOYBUTTONDOWN:
                    speaker->say("mi-aou");
                    break;

                case SDL_JOYHATMOTION:
                    if(event.jhat.value == SDL_HAT_LEFT)
                        speaker->say("left");
                    else if(event.jhat.value == SDL_HAT_UP)
                        speaker->say("up");
                    else if(event.jhat.value == SDL_HAT_DOWN)
                        speaker->say("down");
                    else if(event.jhat.value == SDL_HAT_RIGHT)
                        speaker->say("right");
                    break;

                case SDL_JOYAXISMOTION:
//                    if(event.jaxis.axis == 0 && event.jaxis.value < - 2000)
//                        speaker->say("left");
//                        else
//                    if(event.jaxis.axis == 0 && event.jaxis.value > 2000)
//                        speaker->say("right");
//                        else
//                    if(event.jaxis.axis == 1 && event.jaxis.value < - 2000)
//                        speaker->say("down");
//                        else
//                    if(event.jaxis.axis == 1 && event.jaxis.value > 2000)
//                        speaker->say("up");

                    break;
                case SDL_KEYDOWN:
                speaker->say(string("hi"));
                /* handle keyboard stuff here */
                break;

            }

            if(event.type == SDL_QUIT)
            {
                break;

            }

      }


   }

   SDL_Quit();

    speaker->say("What is your age");
    int age;
    cin >> age;

    if(age < 50)
    {

        speaker->say("Your are very young");
    }
    else
        {
            speaker->say("Your are very old");
    }


    return 0;
}

