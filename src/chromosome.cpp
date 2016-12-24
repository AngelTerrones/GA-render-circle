#include <iostream>
#include "chromosome.hpp"

Chromosome::Chromosome(){
    listCircle = NULL;
    length     = 0;
    fitness    = 0;
}

Chromosome::~Chromosome(){
    Delete();
}

void Chromosome::Create(int nCircles){
    Delete();
    listCircle = new Circle[nCircles];
    length     = nCircles;
    fitness    = 0;
}

void Chromosome::Delete(){
    if(listCircle != NULL)
        delete[] listCircle;

    listCircle = NULL;
    length     = 0;
    fitness    = 0;
}

int Chromosome::Length(){
    return length;
}

void Chromosome::Clone(Chromosome *copy){
    if(copy == NULL){
        std::cerr << "Unable to copy chromosome. Abort" << std::endl;
        exit(-1);
    }
    if(length == 0)
        return;
    copy->Create(length);
    copy->fitness = fitness;

    for(int i = 0; i < length; i++)
        copy->listCircle[i] = listCircle[i];
}

void Chromosome::Mutate(){
    // mutate a random field from a random circle
    int circle = rand() % length;
    int field = rand() % 6; // RGBA + center + radius
    switch (field) {
        case 0:
            listCircle[circle].red(listCircle[circle].red() + rand()%51 - 25);
            break;
        case 1:
            listCircle[circle].green(listCircle[circle].green() + rand()%51 - 25);
            break;
        case 2:
            listCircle[circle].blue(listCircle[circle].blue() + rand()%51 - 25);
            break;
        case 3:
            listCircle[circle].alpha(listCircle[circle].alpha() + rand()%51 - 25);
            break;
        case 4:{
            int tmp = listCircle[circle].radius() + rand() % 21 - 10;
            int minDim = min(listCircle[circle].xMax(), listCircle[circle].yMax())/2;
            listCircle[circle].radius() = max(0, min(tmp, minDim));
            break;
        }
        default:
            if(rand() % 2){
                int tmp = listCircle[circle].center().x() + rand() % 21 - 10;
                listCircle[circle].center().rx() = max(0, min(tmp, listCircle[circle].xMax()));
            }else{
                int tmp = listCircle[circle].center().y() + rand() % 21 - 10;
                listCircle[circle].center().ry() = max(0, min(tmp, listCircle[circle].yMax()));
            }
            break;
    }
}

Circle *Chromosome::DNA(){
    return listCircle;
}

unsigned long long &Chromosome::Fitness(){
    return fitness;
}
