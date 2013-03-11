#include "ConsoleSpeaker.h"
#include <iostream>

using namespace std;

ConsoleSpeaker::ConsoleSpeaker()
{
    //ctor
}

ConsoleSpeaker::~ConsoleSpeaker()
{
    //dtor
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
void ConsoleSpeaker::say(std::string text)
{
    cout << text;

}
