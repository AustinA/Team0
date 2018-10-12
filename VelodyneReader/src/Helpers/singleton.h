#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
struct Singleton
{

    /// call with:
    /// T* myThing = &Singleton<T>::Instance();
    static auto &Instance()
    {
        static T single;
        return single;
    }

    /// do not instanciate pls.
    ///
    ///
    /// No Seriously
    Singleton() = delete;
    Singleton(const Singleton &other) = delete;
    Singleton & operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

};


#endif // SINGLETON_H
