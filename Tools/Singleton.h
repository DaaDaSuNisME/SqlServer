#pragma once

template <typename T>
class Singleton
{
public:
    static T * instance(){
        static T instance;
        return &instance;
    }

private:
    Singleton() {}
    Singleton(const Singleton<T> &)=delete;
    Singleton<T> & operator = (const Singleton<T> &)=delete;
    ~Singleton() {}
};