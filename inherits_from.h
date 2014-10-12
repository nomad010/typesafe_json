#pragma once

#include <cstdio>
#include <functional>

template <typename T>
class InheritsFrom : public T
{
};

template <>
class InheritsFrom<bool>
{
protected:
    bool value;
public:
    operator bool()
    {
        return value;
    }
    
    InheritsFrom(const bool& value) : value(value) 
    {
    }
    
    InheritsFrom()
    {
    }
    ~InheritsFrom()
    {
    }
    
    friend bool operator<(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value < rhs.value;
    }
    friend bool operator<(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value < rhs;
    }
    friend bool operator<(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs < rhs.value;
    }
    
    friend bool operator<=(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value <= rhs.value;
    }
    friend bool operator<=(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value <= rhs;
    }
    friend bool operator<=(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs <= rhs.value;
    }
    
    friend bool operator==(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value == rhs.value;
    }
    friend bool operator==(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value == rhs;
    }
    friend bool operator==(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs == rhs.value;
    }
    
    friend bool operator>=(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value >= rhs.value;
    }
    friend bool operator>=(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value >= rhs;
    }
    friend bool operator>=(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs >= rhs.value;
    }
    
    friend bool operator>(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value > rhs.value;
    }
    friend bool operator>(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value > rhs;
    }
    friend bool operator>(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs > rhs.value;
    }
    
    friend bool operator!=(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs.value != rhs.value;
    }
    friend bool operator!=(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return lhs.value != rhs;
    }
    friend bool operator!=(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return lhs != rhs.value;
    }
    
    friend InheritsFrom<bool> operator+(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value + rhs.value);
    }
    friend InheritsFrom<bool> operator+(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value + rhs);
    }
    friend InheritsFrom<bool> operator+(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs + rhs.value);
    }
    
    friend InheritsFrom<bool> operator-(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value - rhs.value);
    }
    friend InheritsFrom<bool> operator-(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value - rhs);
    }
    friend InheritsFrom<bool> operator-(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs - rhs.value);
    }
    
    friend InheritsFrom<bool> operator*(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value*rhs.value);
    }
    friend InheritsFrom<bool> operator*(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value*rhs);
    }
    friend InheritsFrom<bool> operator*(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs*rhs.value);
    }
    
    friend InheritsFrom<bool> operator/(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value/rhs.value);
    }
    friend InheritsFrom<bool> operator/(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value/rhs);
    }
    friend InheritsFrom<bool> operator/(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs/rhs.value);
    }
    
    friend InheritsFrom<bool> operator%(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value % rhs.value);
    }
    
    friend InheritsFrom<bool> operator%(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value % rhs);
    }
    friend InheritsFrom<bool> operator%(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs % rhs.value);
    }
    
//     InheritsFrom<bool>& operator++()
//     {
//         ++value;
//         return *this;
//     }
//     
//     InheritsFrom<bool> operator++(int)
//     {
//         InheritsFrom<bool> cpy(*this);
//         ++value;
//         return cpy;
//     }
    
    friend InheritsFrom<bool> operator-(const InheritsFrom<bool>& lhs)
    {
        return InheritsFrom<bool>(-lhs.value);
    }
    
    friend InheritsFrom<bool> operator!(const InheritsFrom<bool>& lhs)
    {
        return InheritsFrom<bool>(!lhs.value);
    }
    
    friend InheritsFrom<bool> operator&&(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value && rhs.value);
    }
    friend InheritsFrom<bool> operator&&(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value && rhs);
    }
    friend InheritsFrom<bool> operator&&(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs && rhs.value);
    }
    
    InheritsFrom<bool> operator~()
    {
        return InheritsFrom<bool>(~value);
    }
    
    friend InheritsFrom<bool> operator&(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value & rhs.value);
    }
    friend InheritsFrom<bool> operator&(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value & rhs);
    }
    friend InheritsFrom<bool> operator&(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs & rhs.value);
    }
    
    friend InheritsFrom<bool> operator|(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value | rhs.value);
    }
    friend InheritsFrom<bool> operator|(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value | rhs);
    }
    friend InheritsFrom<bool> operator|(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs | rhs.value);
    }
    
    friend InheritsFrom<bool> operator^(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value ^ rhs.value);
    }
    friend InheritsFrom<bool> operator^(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value ^ rhs);
    }
    friend InheritsFrom<bool> operator^(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs ^ rhs.value);
    }
    
    friend InheritsFrom<bool> operator<<(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value << rhs.value);
    }
    friend InheritsFrom<bool> operator<<(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value << rhs);
    }
    friend InheritsFrom<bool> operator<<(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs << rhs.value);
    }
    
    friend InheritsFrom<bool> operator>>(const InheritsFrom<bool>& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs.value >> rhs.value);
    }
    friend InheritsFrom<bool> operator>>(const InheritsFrom<bool>& lhs, const bool& rhs)
    {
        return InheritsFrom<bool>(lhs.value >> rhs);
    }
    friend InheritsFrom<bool> operator>>(const bool& lhs, const InheritsFrom<bool>& rhs)
    {
        return InheritsFrom<bool>(lhs >> rhs.value);
    }
};

#define CreateInheritsFromIntegralPrimitive(CType) \
template <> \
class InheritsFrom<CType> \
{\
protected:\
    CType value;\
public:\
    \
    operator CType()\
    {\
        return value;\
    }\
    \
    InheritsFrom(const CType& value) : value(value) \
    {\
    }\
    \
    InheritsFrom()\
    {\
    }\
    ~InheritsFrom()\
    {\
    }\
    \
    friend bool operator<(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value < rhs.value;\
    }\
    friend bool operator<(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value < rhs;\
    }\
    friend bool operator<(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs < rhs.value;\
    }\
    \
    friend bool operator<=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value <= rhs.value;\
    }\
    friend bool operator<=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value <= rhs;\
    }\
    friend bool operator<=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs <= rhs.value;\
    }\
    \
    friend bool operator==(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value == rhs.value;\
    }\
    friend bool operator==(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value == rhs;\
    }\
    friend bool operator==(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs == rhs.value;\
    }\
    \
    friend bool operator>=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value >= rhs.value;\
    }\
    friend bool operator>=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value >= rhs;\
    }\
    friend bool operator>=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs >= rhs.value;\
    }\
    \
    friend bool operator>(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value > rhs.value;\
    }\
    friend bool operator>(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value > rhs;\
    }\
    friend bool operator>(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs > rhs.value;\
    }\
    \
    friend bool operator!=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value != rhs.value;\
    }\
    friend bool operator!=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value != rhs;\
    }\
    friend bool operator!=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs != rhs.value;\
    }\
    \
    friend InheritsFrom<CType> operator+(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value + rhs.value);\
    }\
    friend InheritsFrom<CType> operator+(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value + rhs);\
    }\
    friend InheritsFrom<CType> operator+(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs + rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value - rhs.value);\
    }\
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value - rhs);\
    }\
    friend InheritsFrom<CType> operator-(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs - rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator*(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value*rhs.value);\
    }\
    friend InheritsFrom<CType> operator*(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value*rhs);\
    }\
    friend InheritsFrom<CType> operator*(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs*rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator/(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value/rhs.value);\
    }\
    friend InheritsFrom<CType> operator/(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value/rhs);\
    }\
    friend InheritsFrom<CType> operator/(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs/rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator%(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value % rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator%(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value % rhs);\
    }\
    friend InheritsFrom<CType> operator%(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs % rhs.value);\
    }\
    \
    InheritsFrom<CType>& operator++()\
    {\
        ++value;\
        return *this;\
    }\
    \
    InheritsFrom<CType> operator++(int)\
    {\
        InheritsFrom<CType> cpy(*this);\
        ++value;\
        return cpy;\
    }\
    \
    InheritsFrom<CType>& operator--()\
    {\
        --value;\
        return *this;\
    }\
    \
    InheritsFrom<CType> operator--(int)\
    {\
        InheritsFrom<CType> cpy(*this);\
        --value;\
        return cpy;\
    }\
    \
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs)\
    {\
        return InheritsFrom<CType>(-lhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator!(const InheritsFrom<CType>& lhs)\
    {\
        return InheritsFrom<CType>(!lhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator&&(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value && rhs.value);\
    }\
    friend InheritsFrom<CType> operator&&(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value && rhs);\
    }\
    friend InheritsFrom<CType> operator&&(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs && rhs.value);\
    }\
    \
    InheritsFrom<CType> operator~()\
    {\
        return InheritsFrom<CType>(~value);\
    }\
    \
    friend InheritsFrom<CType> operator&(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value & rhs.value);\
    }\
    friend InheritsFrom<CType> operator&(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value & rhs);\
    }\
    friend InheritsFrom<CType> operator&(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs & rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator|(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value | rhs.value);\
    }\
    friend InheritsFrom<CType> operator|(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value | rhs);\
    }\
    friend InheritsFrom<CType> operator|(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs | rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator^(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value ^ rhs.value);\
    }\
    friend InheritsFrom<CType> operator^(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value ^ rhs);\
    }\
    friend InheritsFrom<CType> operator^(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs ^ rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator<<(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value << rhs.value);\
    }\
    friend InheritsFrom<CType> operator<<(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value << rhs);\
    }\
    friend InheritsFrom<CType> operator<<(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs << rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator>>(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value >> rhs.value);\
    }\
    friend InheritsFrom<CType> operator>>(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value >> rhs);\
    }\
    friend InheritsFrom<CType> operator>>(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs >> rhs.value);\
    }\
};

#define CreateInheritsFromFloatingPointPrimitive(CType) \
template <> \
class InheritsFrom<CType> \
{\
protected:\
    CType value;\
public:\
    \
    operator CType()\
    {\
        return value;\
    }\
    \
    InheritsFrom(const CType& value) : value(value) \
    {\
    }\
    \
    InheritsFrom()\
    {\
    }\
    ~InheritsFrom()\
    {\
    }\
    \
    friend bool operator<(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value < rhs.value;\
    }\
    friend bool operator<(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value < rhs;\
    }\
    friend bool operator<(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs < rhs.value;\
    }\
    \
    friend bool operator<=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value <= rhs.value;\
    }\
    friend bool operator<=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value <= rhs;\
    }\
    friend bool operator<=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs <= rhs.value;\
    }\
    \
    friend bool operator==(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value == rhs.value;\
    }\
    friend bool operator==(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value == rhs;\
    }\
    friend bool operator==(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs == rhs.value;\
    }\
    \
    friend bool operator>=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value >= rhs.value;\
    }\
    friend bool operator>=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value >= rhs;\
    }\
    friend bool operator>=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs >= rhs.value;\
    }\
    \
    friend bool operator>(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value > rhs.value;\
    }\
    friend bool operator>(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value > rhs;\
    }\
    friend bool operator>(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs > rhs.value;\
    }\
    \
    friend bool operator!=(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs.value != rhs.value;\
    }\
    friend bool operator!=(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return lhs.value != rhs;\
    }\
    friend bool operator!=(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return lhs != rhs.value;\
    }\
    \
    friend InheritsFrom<CType> operator+(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value + rhs.value);\
    }\
    friend InheritsFrom<CType> operator+(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value + rhs);\
    }\
    friend InheritsFrom<CType> operator+(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs + rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value - rhs.value);\
    }\
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value - rhs);\
    }\
    friend InheritsFrom<CType> operator-(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs - rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator*(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value*rhs.value);\
    }\
    friend InheritsFrom<CType> operator*(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value*rhs);\
    }\
    friend InheritsFrom<CType> operator*(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs*rhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator/(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value/rhs.value);\
    }\
    friend InheritsFrom<CType> operator/(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value/rhs);\
    }\
    friend InheritsFrom<CType> operator/(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs/rhs.value);\
    }\
    \
    InheritsFrom<CType>& operator++()\
    {\
        ++value;\
        return *this;\
    }\
    \
    InheritsFrom<CType> operator++(int)\
    {\
        InheritsFrom<CType> cpy(*this);\
        ++value;\
        return cpy;\
    }\
    \
    InheritsFrom<CType>& operator--()\
    {\
        --value;\
        return *this;\
    }\
    \
    InheritsFrom<CType> operator--(int)\
    {\
        InheritsFrom<CType> cpy(*this);\
        --value;\
        return cpy;\
    }\
    \
    friend InheritsFrom<CType> operator-(const InheritsFrom<CType>& lhs)\
    {\
        return InheritsFrom<CType>(-lhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator!(const InheritsFrom<CType>& lhs)\
    {\
        return InheritsFrom<CType>(!lhs.value);\
    }\
    \
    friend InheritsFrom<CType> operator&&(const InheritsFrom<CType>& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value && rhs.value);\
    }\
    friend InheritsFrom<CType> operator&&(const InheritsFrom<CType>& lhs, const CType& rhs)\
    {\
        return InheritsFrom<CType>(lhs.value && rhs);\
    }\
    friend InheritsFrom<CType> operator&&(const CType& lhs, const InheritsFrom<CType>& rhs)\
    {\
        return InheritsFrom<CType>(lhs && rhs.value);\
    }\
    \
};

CreateInheritsFromIntegralPrimitive(char);
CreateInheritsFromIntegralPrimitive(char16_t);
CreateInheritsFromIntegralPrimitive(char32_t);
CreateInheritsFromIntegralPrimitive(wchar_t);
CreateInheritsFromIntegralPrimitive(signed char);
CreateInheritsFromIntegralPrimitive(short int);
CreateInheritsFromIntegralPrimitive(int);
CreateInheritsFromIntegralPrimitive(long int);
CreateInheritsFromIntegralPrimitive(long long int);
CreateInheritsFromIntegralPrimitive(unsigned char);
CreateInheritsFromIntegralPrimitive(unsigned short int);
CreateInheritsFromIntegralPrimitive(unsigned int);
CreateInheritsFromIntegralPrimitive(unsigned long int);
CreateInheritsFromIntegralPrimitive(unsigned long long int);
CreateInheritsFromFloatingPointPrimitive(float);
CreateInheritsFromFloatingPointPrimitive(double);
CreateInheritsFromFloatingPointPrimitive(long double);

#undef CreateInheritsFromIntegralPrimitive
#undef CreateInheritsFromFloatingPointPrimitive
