#include "InputEventHandler.h"
#include <vector>



using namespace std;





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
    this->inputListeners.push_back(inputListener);
}


void InputEventHandler::notify(int inputEvent)
{
    for(vector<InputListener*>::iterator i = inputListeners.begin(); i != inputListeners.end();++i)
    {
        (*i)->notify(inputEvent);
    }

}

