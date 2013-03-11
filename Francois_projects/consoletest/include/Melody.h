#ifndef MELODY_H
#define MELODY_H

#include <vector.h>

class Melody
{


    public:
        Melody();
        virtual ~Melody();

        /*! add a note to the melody
            \param n an integer that represents the note
            \return nothing
        */
        void addNote(int n);
    protected:
    private:
        vector<int> notes;
};

#endif // MELODY_H
