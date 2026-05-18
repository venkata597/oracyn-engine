#include <iostream>
#include "../include/oracyn.hpp"


int main(){
    Window window("Test Window");
    while(window.isOpen()){
        window.handleEvents();
    }
}