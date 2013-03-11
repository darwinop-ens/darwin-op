#ifndef INPUTEVENTHANDLER_H
#define INPUTEVENTHANDLER_H

#include <vector>
#include "InputListener.h"



enum
{
    INPUTEVENT_UP,
    INPUTEVENT_DOWN,
    INPUTEVENT_LEFT,
    INPUTEVENT_RIGHT,
    INPUTEVENT_ACTION
};

class InputEventHandler
{



    public:

        InputEventHandler();
        virtual ~InputEventHandler();
        void addInputListener(InputListener* inputListener);
        virtual void checkInputs() = 0;
    protected:
        void notify(int inputEvent);
    private:
        std::vector<InputListener*> inputListeners;

};

#endif // INPUTEVENTHANDLER_H
