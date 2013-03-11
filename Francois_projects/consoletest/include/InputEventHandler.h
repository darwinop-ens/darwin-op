#ifndef INPUTEVENTHANDLER_H
#define INPUTEVENTHANDLER_H


class InputEventHandler
{
    enum InputEvent
    {
        INPUTEVENT_UP,
        INPUTEVENT_DOWN,
        INPUTEVENT_LEFT,
        INPUTEVENT_RIGHT,
        INPUTEVENT_ACTION
    };


    public:
        InputEventHandler();
        virtual ~InputEventHandler();
        void addInputListener(InputListener* inputListener);
        virtual void checkInputs() = 0;
    protected:
    private:
        InputListener* inputListener;
        void notify(InputEvent inputEvent);
};

#endif // INPUTEVENTHANDLER_H
