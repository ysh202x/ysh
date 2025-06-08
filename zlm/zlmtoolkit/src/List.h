#ifndef __YSH_LIST_H__
#define __YSH_LIST_H__

#include <list>

template<typename T>
class List : public std::list<T>
{
public:
    /*template<typename... ARG> 是C++中的可变参数模板（Variadic Template）声明。
    具体分析：
    template 关键字表示这是一个模板声明
    typename... 中的 ... 表示这是一个可变参数模板 */
    template<typename... ARG>
    List(ARG &&... args) : std::list<T>(std::forward<ARG>(args)...) {}
    
    ~List() = default;

    void append(List <T> &other)
    {
        if(other.empty())
        {
            return;
        }
        this->insert(this->end(), other.begin(), other.end());
        other.clear();
    }

    template<typename FUNC>
    void for_each(FUNC &&func)
    {
        for(auto &item : *this)
        {
            func(item);
        }
    }

    template<typename FUNC>
    void for_each(FUNC &&func) const
    {
        for(auto &item : *this)
        {
            func(item);
        }
    }

    void swap(List<T> &other)
    {
        std::list<T>::swap(other);
    }
};


#endif
