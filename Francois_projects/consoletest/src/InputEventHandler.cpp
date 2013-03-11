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


void InputEventHandler::notify(int inputEvent)
{
    this->inputListener->notify(inputEvent);
}

