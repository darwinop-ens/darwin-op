#include "JoystickEventHandler.h"
#include "SDL/SDL.h"
#include <iostream>


using namespace std;


JoystickEventHandler::JoystickEventHandler()
{
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
        return;
    }




    SDL_Joystick *joystick;
    joystick = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
}

JoystickEventHandler::~JoystickEventHandler()
{
    //dtor
}








void JoystickEventHandler::checkInputs()
{
        SDL_Event event;

        while(1)
        {


            // Check for messages
              if (SDL_PollEvent(&event))
              {

                  switch(event.type)
                    {
                        case SDL_JOYBUTTONDOWN:
                            notify(INPUTEVENT_ACTION);
                            break;

                        case SDL_JOYHATMOTION:
                            if(event.jhat.value == SDL_HAT_LEFT)
                                notify(INPUTEVENT_LEFT);
                            else if(event.jhat.value == SDL_HAT_UP)
                                notify(INPUTEVENT_UP);
                            else if(event.jhat.value == SDL_HAT_DOWN)
                                notify(INPUTEVENT_DOWN);
                            else if(event.jhat.value == SDL_HAT_RIGHT)
                                notify(INPUTEVENT_RIGHT);
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
                            notify(INPUTEVENT_ACTION);
                        /* handle keyboard stuff here */
                        break;

                    }
              }

              if(event.type == SDL_QUIT)
                {
                    break;

                }
        }
}
