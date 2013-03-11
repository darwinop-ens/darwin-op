#include "InputEventHandler.h"

InputEventHandler::InputEventHandler()
{
    //ctor
}

InputEventHandler::~InputEventHandler()
{
    //dtor
}


void InputEventHandler::addInputListener(InputListener* inputListener)
{
    this->inputListener = inputListener;
}


void InputEventHandler::notify(InputEvent inputEvent)
{
    this->inputListener->notify(inputEvent);
}

