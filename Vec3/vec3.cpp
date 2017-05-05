#include "vec3.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>

using namespace std;

vec3::vec3()
{
    componants[0] = 0;
    componants[1] = 0;
    componants[2] = 0;
}
vec3::vec3(double a, double b, double c)
{
    componants[0] = a;
    componants[1] = b;
    componants[2] = c;
}


vec3 &vec3::operator-=(const vec3 &rhs)
{
    this->componants[0] -= rhs.componants[0];
    this->componants[1] -= rhs.componants[1];
    this->componants[2] -= rhs.componants[2];
    return *this;
}

vec3 &vec3::operator+=(const vec3 &rhs)
{
    componants[0] += rhs.componants[0];
    componants[1] += rhs.componants[1];
    componants[2] += rhs.componants[2];
    return *this;
}

vec3 &vec3::operator *=(const double &c){
    componants[0] *= c;
    componants[1] *= c;
    componants[2] *= c;
    return *this;
}


vec3 &vec3::operator /=(const double &c){
    componants[0] /= c;
    componants[1] /= c;
    componants[2] /= c;
    return *this;
}

vec3 &vec3::operator /=(const vec3 &rhs){
    componants[0] /= rhs.componants[0];
    componants[1] /= rhs.componants[1];
    componants[2] /= rhs.componants[2];
    return *this;
}

vec3 &vec3::operator=(const vec3 &rhs)
{
    componants[0] = rhs.componants[0];
    componants[1] = rhs.componants[1];
    componants[2] = rhs.componants[2];
    return *this;
}

vec3 &vec3::operator=(const double &s)
{
    componants[0] = s;
    componants[1] = s;
    componants[2] = s;
    return *this;
}



vec3 vec3::randint(int min, int max)
{
    componants[0] = rand()%(max-min + 1) + min;
    componants[1] = rand()%(max-min + 1) + min;
    componants[2] = rand()%(max-min + 1) + min;
    return *this;
}

void vec3::normalize(){
    *this = *this/length();
}

void vec3::print(){
    cout << "[" << componants[0] << "," << componants[1] << "," << componants[2] << "]" << endl;
}


double vec3::lengthSquared()
{
    return componants[0]*componants[0] + componants[1]*componants[1] + componants[2]*componants[2];
}

double vec3::length()
{
   return sqrt(lengthSquared());
}


