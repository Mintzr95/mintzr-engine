#pragma once
#include <cmath>
#include <algorithm>
namespace mju {
struct Vec2 { float x=0, y=0; constexpr Vec2()=default; constexpr Vec2(float X,float Y):x(X),y(Y){}; constexpr Vec2 operator+(const Vec2&r)const{return{x+r.x,y+r.y};}; constexpr Vec2 operator-(const Vec2&r)const{return{x-r.x,y-r.y};}; constexpr Vec2 operator*(float s)const{return{x*s,y*s};}; Vec2& operator+=(const Vec2&r){x+=r.x;y+=r.y;return *this;} float length()const{return std::sqrt(x*x+y*y);} };
struct Color { float r=1,g=1,b=1,a=1; };
inline float clamp(float v,float lo,float hi){return std::max(lo,std::min(hi,v));}
}
