#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

template<class T>
using Type = typename decay<typename remove_reference<T>::type>::type;

struct Variant
{
    bool isNull() const { return !p; }
    bool notNull() const { return p; }



    template<class V> bool is() const
    {
        typedef Type<V> T;

        auto derived = dynamic_cast<Derived<T>*> (p);

        return derived;
    }

    template<class V>
    Type<V>& as()
    {
        typedef Type<V> T;

        auto derived = dynamic_cast<Derived<T>*> (p);

        if (!derived)
            throw bad_cast();

        return derived->val;
    }

    template<typename V> Variant(V&& val)
            : p(new Derived<Type<V>>(forward<V>(val)))
    {

    }

    Variant()
        : p(nullptr)
    {

    }

    Variant(Variant& invar)
        : p(invar.clone())
    {

    }

    Variant(Variant&& invar)
        : p(invar.p)
    {
        invar.p = nullptr;
    }

    Variant(const Variant& invar)
        : p(invar.clone())
    {

    }

    Variant(const Variant&& invar)
        : p(invar.clone())
    {

    }

    ~Variant()
    {
        if (p)
            delete p;
    }

    template<class V>
    operator V()
    {
        return as<Type<V>>();
    }

    Variant& operator=(const Variant& variant)
    {
        if (p == variant.p)
            return *this;

        auto old_ptr = p;

        p = variant.clone();

        if (old_ptr)
            delete old_ptr;

        return *this;
    }

    Variant& operator=(Variant&& variant)
    {
        if (p == variant.p)
            return *this;

        swap(p, variant.p);

        return *this;
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
        template<typename V> Derived(V&& val) : val(forward<V>(val)) { }

        T val;

        Base* clone() const { return new Derived<T>(val); }
    };

    Base* clone() const
    {
        if (p)
            return p->clone();
        else
            return nullptr;
    }

    Base* p;
};

int main()
{
    Variant variant;
    variant = 3;
    int test = variant;
    std::cout << test << std::endl;

    variant = 2.5f;
    float test2 = variant;
    std::cout << test2 << std::endl;

    string s4 = "test";
    variant = s4;
    string test3 = variant;
    std::cout << test3 << std::endl;


    Variant my;
    vector<int> vec3(3,5);
    my = vec3;

    std::cout << my.is<vector<int>>() << std::endl;

    std::cout << my.as<vector<int>>()[0] << std::endl;

    variant = my;

    variant.as<vector<int>>()[1] = 2;

    std::cout << variant.as<vector<int>>()[1] << std::endl;
    std::cout << my.as<vector<int>>()[1] << std::endl;

}