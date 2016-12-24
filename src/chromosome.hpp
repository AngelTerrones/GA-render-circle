#ifndef CROMOSOME_H
#define CROMOSOME_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QList>
#include <QPoint>
#include <algorithm>

using namespace std;

class Circle{
    public:
        Circle(){
            r       = rand() % 255;
            g       = rand() % 255;
            b       = rand() % 255;
            a       = rand() % 255;
            _xMax   = 0;
            _yMax   = 0;
            _center = QPoint(0, 0);
            _radius = 0;
        }
        virtual ~Circle(){}
        void init(int _xMax, int _yMax){
            this->_xMax = _xMax;
            this->_yMax = _yMax;
            _center.rx() = rand() % _xMax;
            _center.ry() = rand() % _yMax;
            _radius = rand() % (max(_xMax, _yMax)/2);
        }
        // getter
        unsigned char red()    {return r;}
        unsigned char green()  {return g;}
        unsigned char blue()   {return b;}
        unsigned char alpha()  {return a;}
        QPoint &center()       {return _center;}
        unsigned int &radius() {return _radius;}
        int xMax()             {return _xMax;}
        int yMax()             {return _yMax;}
        // setter
        void red   (unsigned char color){r = color;}
        void green (unsigned char color){g = color;}
        void blue  (unsigned char color){b = color;}
        void alpha (unsigned char color){a = color;}
        // Overload
        Circle &operator =(const Circle A){
            if(this != &A){
                r       = A.r;
                g       = A.g;
                b       = A.b;
                a       = A.a;
                _xMax   = A._xMax;
                _yMax   = A._yMax;
                _center = A._center;
                _radius = A._radius;
            }
            return *this;
        }
    private:
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        int _xMax;
        int _yMax;
        QPoint _center;
        unsigned int _radius;
};

class Chromosome
{
    public:
        Chromosome();
        virtual ~Chromosome();
        void     Create(int nCircles);
        void     Delete();
        int      Length();
        void     Clone(Chromosome *copy);
        void     Mutate();
        Circle  *DNA();
        unsigned long long &Fitness();
    private:
        Circle *listCircle;
        unsigned long long fitness;
        int length;
};

#endif // CROMOSOME_H
