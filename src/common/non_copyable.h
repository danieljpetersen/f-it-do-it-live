#ifndef FI_NONCOPYABLE_H_INCLUDED
#define FI_NONCOPYABLE_H_INCLUDED

struct Non_Copyable
{
    Non_Copyable() = default;
    Non_Copyable(const Non_Copyable&) = delete;
    Non_Copyable& operator=(const Non_Copyable&) = delete;
};

#endif // NONCOPYABLE_H_INCLUDED
