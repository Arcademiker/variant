#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

template<class T>
using StorageType = typename decay<typename remove_reference<T>::type>::type;

struct Any
{
    bool is_null() const { return !ptr; }
    bool not_null() const { return ptr; }

    template<typename U> Any(U&& value)
        : ptr(new Derived<StorageType<U>>(forward<U>(value)))
    {

    }

    template<class U> bool is() const
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        return derived;
    }

    template<class U>
    StorageType<U>& as()
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        if (!derived)
            throw bad_cast();

        return derived->value;
    }

    template<class U>
    operator U()
    {
        return as<StorageType<U>>();
    }

    Any()
        : ptr(nullptr)
    {

    }

    Any(Any& that)
        : ptr(that.clone())
    {

    }

    Any(Any&& that)
        : ptr(that.ptr)
    {
        that.ptr = nullptr;
    }

    Any(const Any& that)
        : ptr(that.clone())
    {

    }

    Any(const Any&& that)
        : ptr(that.clone())
    {

    }

    Any& operator=(const Any& a)
    {
        if (ptr == a.ptr)
            return *this;

        auto old_ptr = ptr;

        ptr = a.clone();

        if (old_ptr)
            delete old_ptr;

        return *this;
    }

    Any& operator=(Any&& a)
    {
        if (ptr == a.ptr)
            return *this;

        swap(ptr, a.ptr);

        return *this;
    }

    ~Any()
    {
        if (ptr)
            delete ptr;
    }

private:
    struct Base
    {
        virtual ~Base() {}

        virtual Base* clone() const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U> Derived(U&& value) : value(forward<U>(value)) { }

        T value;

        Base* clone() const { return new Derived<T>(value); }
    };

    Base* clone() const
    {
        if (ptr)
            return ptr->clone();
        else
            return nullptr;
    }

    Base* ptr;
};

int main()
{
    Any n;
    assert(n.is_null());

    string s1 = "foo";

    Any a1 = s1;

    assert(a1.not_null());
    assert(a1.is<string>());
    assert(!a1.is<int>());

    Any a2(a1);

    assert(a2.not_null());
    assert(a2.is<string>());
    assert(!a2.is<int>());

    string s2 = a2;

    assert(s1 == s2);

    //n = 3;
    //int test = n;
    //std::cout << test << std::endl;

    //n = 2.5f;
    //float test2 = n;
    //std::cout << test2 << std::endl;

    string s4 = "test";
    n = s4;
    string test3 = n;
    //assert(n.not_null());
    //assert(n.is<string>());
    //assert(!n.is<int>());
    std::cout << test3 << std::endl;


    Any my;
    vector<int> vec3(3,1);
    my = vec3;

    std::cout << my.is<vector<int>>() << std::endl;

    std::cout << my.as<vector<int>>()[0] << std::endl;


}