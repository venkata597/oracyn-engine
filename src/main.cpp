#include <iostream>
#include "../include/oracyn.hpp"


int main(){
    AppWindow window("Test Engine");
    while(window.isOpen()){
        window.handleEvents();
        
        window.swapBuffers();
    }
}