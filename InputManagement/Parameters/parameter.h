#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>
struct NotImplementedException: public std::exception {
    const char* what () const throw () {
        return "Tried to get parameter with wrong type";
    }
};

struct UnsetException: public std::exception{
    const char* what () const throw (){
        return "Tried to get unset parameter";
    }
};

class ParameterInterface
{
public:
    virtual ~ParameterInterface(){};
    bool isSet() const {return flag;}
    void verify() const {if (!isSet()){throw UnsetException();}}
    virtual void read(const std::string& token) = 0;
    virtual void copyTo(int&){throw NotImplementedException();};
    virtual void copyTo(bool&){throw NotImplementedException();};
    virtual void copyTo(double&){throw NotImplementedException();};
    virtual void copyTo(std::string&){throw NotImplementedException();};
protected:
    bool flag = false;
};

template <typename T>
class ParameterBase: public ParameterInterface
{
public:
    ParameterBase(){};
    virtual ~ParameterBase(){};
    virtual void read(const std::string& token) = 0;
protected:
    T _get() const{
        if (!isSet())
            throw UnsetException();
        return value;
    };
    void set(const T& value){
        if (!isSet()){
            this->value = value;
            flag = true;
        } else {
            throw UnsetException();
        }
    };
private:
    T value;
};

template <typename T>
class Parameter: public ParameterBase<T>
{
public:
    Parameter(){};
};

template <>
class Parameter<int>: public ParameterBase<int>
{
public:
    void read(const std::string &token) override {set(static_cast<int>(std::stod(token)));}
    void copyTo(int& var){var = _get();}
};

template <>
class Parameter<double>: public ParameterBase<double>
{
public:
    void read(const std::string &token) override {set(std::stod(token));}
    void copyTo(double& var){var = _get();}
    void copyTo(int& var){var = _get();}
};

template <>
class Parameter<std::string>: public ParameterBase<std::string>
{
public:
    void read(const std::string &token) override {set(token);}
    void copyTo(std::string& var){var = _get();}
};

template <>
class Parameter<bool>: public ParameterBase<bool>
{
public:
    void read(const std::string &token) override {set(std::stoi(token));}
    void copyTo(bool& var){var = _get();}
    void copyTo(int& var){var = _get();}
};
#endif /* PARAMETER_H */
