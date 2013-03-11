#ifndef INPUTLISTENERARMLEFT_H
#define INPUTLISTENERARMLEFT_H

#include <InputListener.h>


class InputListenerArmLeft : public InputListener
{
    public:
        InputListenerArmLeft();
        virtual ~InputListenerArmLeft();
        void notify(int inputEvent);

    protected:
    private:
};

#endif // INPUTLISTENERARMLEFT_H
