#pragma once
namespace NBE
{
    template<class T>
    class Singleton
    {
    public:
        static T* getInstancePtr()
        {
            if (s_pInstance == nullptr)
            {
                s_pInstance = new T();
            }
            return s_pInstance;
        }
        static void deleteInstance()
        {
            delete s_pInstance;
            s_pInstance = nullptr;
        }
    protected:
        Singleton()
        {}
        virtual ~Singleton()
        {}
    private:
        static T* s_pInstance;
    };
    template<class T>
    T* Singleton<T>::s_pInstance;
}