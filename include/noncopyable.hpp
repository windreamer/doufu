#ifndef __NONCOPYABLE_HPP__
#define __NONCOPYABLE_HPP__
namespace doufu
{
    class noncopyable
    {
    public:
        noncopyable(){}
        ~noncopyable(){}
    private:
        noncopyable(const noncopyable&);
        noncopyable& operator =(const noncopyable&);
    };
}
#endif

