#include <iostream>
template<template<class> class T> class A { }; //starting state
template<class T> class Y; //transitionals
template<> class Y<int> { //let's check a cache
    Y* p;                                // meaning Y<int>
    Y<char>* q;                          // meaning Y<char>
    A<Y>* a;                             // meaning A<::Y>
    
    class B {
        template<class> friend class Y;  // meaning ::Y

        void checker(B){
            switch (*typeid(B).name())
            {
            case *"USB":
                /* code */
                break;
            case *"Main":
                break;
            case *"Async":
                break;
            default:
                printf("State class %s is not a valid class. Please check your defined parameters again.\n", *typeid(B).name());
                exit -1;
                break;
            }
        }
    };
};