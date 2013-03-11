#include <iostream>
#include <string.h>
#include "VocalSynthesizerSpeaker.h"
#include "AbstractSpeaker.h"
#include "ConsoleSpeaker.h"
#include "InputEventHandler.h"
#include "JoystickEventHandler.h"
#include "InputListenerSpeaker.h"
#include <SDL/SDL.h>


using namespace std;







int main()
{
    AbstractSpeaker* speaker = new VocalSynthesizerSpeaker();
    InputEventHandler* inputEventHandler = new JoystickEventHandler();

    inputEventHandler->addInputListener(new InputListenerSpeaker(speaker));
    inputEventHandler->checkInputs();


   SDL_Quit();

//speaker->say("What is your age");
//    int age;
//    cin >> age;
//
//    if(age < 50)
//    {
//
//        speaker->say("Your are very young");
//    }
//    else
//        {
//            speaker->say("Your are very old");
//    }


    return 0;
}
