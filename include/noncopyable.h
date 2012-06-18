#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__
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

