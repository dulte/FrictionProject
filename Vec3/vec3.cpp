#include "vec3.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>

using namespace std;

vec3::vec3()
{
    components[0] = 0;
    components[1] = 0;
    components[2] = 0;
}
vec3::vec3(double a, double b, double c)
{
    components[0] = a;
    components[1] = b;
    components[2] = c;
}


vec3 &vec3::operator-=(const vec3 &rhs)
{
    this->components[0] -= rhs.components[0];
    this->components[1] -= rhs.components[1];
    this->components[2] -= rhs.components[2];
    return *this;
}

vec3 &vec3::operator+=(const vec3 &rhs)
{
    components[0] += rhs.components[0];
    components[1] += rhs.components[1];
    components[2] += rhs.components[2];
    return *this;
}

vec3 &vec3::operator *=(const double &c){
    components[0] *= c;
    components[1] *= c;
    components[2] *= c;
    return *this;
}


vec3 &vec3::operator /=(const double &c){
    components[0] /= c;
    components[1] /= c;
    components[2] /= c;
    return *this;
}

vec3 &vec3::operator /=(const vec3 &rhs){
    components[0] /= rhs.components[0];
    components[1] /= rhs.components[1];
    components[2] /= rhs.components[2];
    return *this;
}

vec3 &vec3::operator=(const vec3 &rhs)
{
    components[0] = rhs.components[0];
    components[1] = rhs.components[1];
    components[2] = rhs.components[2];
    return *this;
}

vec3 &vec3::operator=(const double &s)
{
    components[0] = s;
    components[1] = s;
    components[2] = s;
    return *this;
}



vec3 vec3::randint(int min, int max)
{
    components[0] = rand()%(max-min + 1) + min;
    components[1] = rand()%(max-min + 1) + min;
    components[2] = rand()%(max-min + 1) + min;
    return *this;
}

void vec3::normalize(){
    *this = *this/length();
}

void vec3::print() const{
    cout << "[" << components[0] << "," << components[1] << "," << components[2] << "]" << endl;
}


double vec3::lengthSquared() const
{
    return components[0]*components[0] + components[1]*components[1] + components[2]*components[2];
}

double vec3::length() const
{
   return sqrt(lengthSquared());
}

double vec3::cross2d(const vec3 &other){
    double x = components[0]*other.components[1];
    double y = -other.components[0]*components[1];
    return x+y;
}
