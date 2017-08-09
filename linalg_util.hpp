#ifndef linalg_util_h
#define linalg_util_h

#include "linalg.h"
#include <iostream>

namespace avl
{
    using namespace linalg::aliases;

    static const float4x4 Identity4x4 = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    static const float3x3 Identity3x3 = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    static const float2x2 Identity2x2 = {{1, 0}, {0, 1}};
    
    static const float4x4 Zero4x4 = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    static const float3x3 Zero3x3 = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    static const float2x2 Zero2x2 = {{0, 0}, {0, 0}};

    template<class T, int M> linalg::vec<T, M>  safe_normalize(const linalg::vec<T,M> & a)  { return a / std::max(T(1E-6), length(a)); }
    template<class T, int N> linalg::mat<T,N,N> inv(const linalg::mat<T,N,N> & a)           { return inverse(a); }
    
    inline float4x4 mult_by_transpose(const float3 & a, const float3 & b)
    {
        float4x4 r;
        r[0][0] = a.x * b.x; r[1][0] = a.y * b.x; r[2][0] = a.z * b.x;
        r[0][1] = a.x * b.y; r[1][1] = a.y * b.y; r[2][1] = a.z * b.y;
        r[0][2] = a.x * b.z; r[1][2] = a.y * b.z; r[2][2] = a.z * b.z;
        return r;
    }

    namespace pretty
    {
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,2> & b) { return a << '{' << b.x << ", " << b.y << '}'; }
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,3> & b) { return a << '{' << b.x << ", " << b.y << ", " << b.z << '}'; }
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,4> & b) { return a << '{' << b.x << ", " << b.y << ", " << b.z << ", " << b.w << '}'; }

        template<class T, int N> std::ostream & operator << (std::ostream & a, const linalg::mat<T,2,N> & b) { return a << '\n' << b.row(0) << '\n' << b.row(1) << '\n'; }
        template<class T, int N> std::ostream & operator << (std::ostream & a, const linalg::mat<T,3,N> & b) { return a << '\n' << b.row(0) << '\n' << b.row(1) << '\n' << b.row(2) << '\n'; }
        template<class T, int N> std::ostream & operator << (std::ostream & a, const linalg::mat<T,4,N> & b) { return a << '\n' << b.row(0) << '\n' << b.row(1) << '\n' << b.row(2) << '\n' << b.row(3) << '\n'; }
    }
    
    /*
    namespace serialize
    {
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,2> & b) { return a << b.x << ' ' << b.y << '}'; }
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,3> & b) { return a << b.x << ' ' << b.y << ' ' << b.z; }
        template<class T> std::ostream & operator << (std::ostream & a, const linalg::vec<T,4> & b) { return a << b.x << ' ' << b.y << ' ' << b.z; }
    }
    */
    
    using namespace pretty;
}

#endif // end linear_algebra_h