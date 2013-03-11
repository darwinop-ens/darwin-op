#ifndef JOYSTICKEVENTHANDLER_H
#define JOYSTICKEVENTHANDLER_H

#include "InputEventHandler.h"



class JoystickEventHandler : public InputEventHandler
{
    public:
        JoystickEventHandler();
        virtual ~JoystickEventHandler();
        void checkInputs();
    protected:
    private:
};

#endif // JOYSTICKEVENTHANDLER_H
