#include "InputListenerSpeaker.h"
#include "InputEventHandler.h"


InputListenerSpeaker::InputListenerSpeaker(AbstractSpeaker* speaker)
{
    this->speaker = speaker;
}

InputListenerSpeaker::~InputListenerSpeaker()
{
    //dtor
}

void InputListenerSpeaker::notify(int inputEvent)
{
    switch(inputEvent)
    {
        case INPUTEVENT_ACTION:
            speaker->say("action");
            break;

        case INPUTEVENT_LEFT:
            speaker->say("left");
            break;

        case INPUTEVENT_RIGHT:
            speaker->say("right");
            break;

        case INPUTEVENT_DOWN:
            speaker->say("down");
            break;

        case INPUTEVENT_UP:
            speaker->say("up");
            break;
    }
}

