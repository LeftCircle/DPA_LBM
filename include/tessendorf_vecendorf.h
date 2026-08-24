//*******************************************************************
//
//   Vector.h
//
// 3D vector class in the namespace pba
//
//  Copyright (c) 2017 Jerry Tessendorf
//
//
//*******************************************************************

#ifndef __PBA_VECTOR_H__
#define __PBA_VECTOR_H__

#include <cmath>
#include <cstdio>

namespace pba
{

//! Vector is a 3D vector class
class Vector
{
  public:

   Vector(){ xyz[0] = xyz[1] = xyz[2] = 0; }

   Vector(const Vector& v)
   { 
      xyz[0] = v.xyz[0];
      xyz[1] = v.xyz[1];
      xyz[2] = v.xyz[2]; 
   }
   
   Vector(const double a, const double b, const double c)
   {
      xyz[0] = a;
      xyz[1] = b;
      xyz[2] = c; 
   }

   ~Vector(){}

   //!  Set all three components
   void set( const float vx, const float vy, const float vz )
   {
      xyz[0] = vx;
      xyz[1] = vy;
      xyz[2] = vz;
   }

   //! Add two vectors together
   const Vector operator+        (const Vector& v) const 
   { 
      return Vector(xyz[0]+v.xyz[0], xyz[1]+v.xyz[1], xyz[2]+v.xyz[2]); 
   }
  
   //! Subtract one vector from another
   const Vector operator-        (const Vector& v) const
   { 
      return Vector(xyz[0]-v.xyz[0], xyz[1]-v.xyz[1], xyz[2]-v.xyz[2]); 
   }

   //! Unary minus
   friend const Vector operator- (const Vector& v)
   { return Vector(-v.xyz[0],-v.xyz[1],-v.xyz[2]); }

   //! Multiplication of a constant with a vector
   friend const Vector operator* (const double w, const Vector& v)
   { return v*w; }
	  
   //! Multiplication of a vector with a constant
   const Vector operator*        (const double v) const
   { return Vector(xyz[0]*v, xyz[1]*v, xyz[2]*v); }

   const Vector operator/        (const double v) const
   { return Vector(xyz[0]/v, xyz[1]/v, xyz[2]/v); }

   //! Inner product
   const double operator*        (const Vector& v) const  
   { return (xyz[0]*v.xyz[0] + xyz[1]*v.xyz[1] + xyz[2]*v.xyz[2]); }
  
   //! cross product
   const Vector operator^        (const Vector& v) const 
   { return Vector(xyz[1]*v.xyz[2] - xyz[2]*v.xyz[1], 
		   xyz[2]*v.xyz[0] - xyz[0]*v.xyz[2], 
		   xyz[0]*v.xyz[1] - xyz[1]*v.xyz[0]); }

   Vector& operator=       (const Vector& v)
   { xyz[0] = v.xyz[0]; xyz[1] = v.xyz[1]; xyz[2] = v.xyz[2]; return *this; }
  
   Vector& operator+=      (const Vector& v)
   { xyz[0] += v.xyz[0]; xyz[1] += v.xyz[1]; xyz[2] += v.xyz[2]; return *this; }
  
   Vector& operator-=      (const Vector& v)
   { xyz[0] -= v.xyz[0]; xyz[1] -= v.xyz[1]; xyz[2] -= v.xyz[2]; return *this; }
  
   Vector& operator*=      (const double v)
   { xyz[0] *= v; xyz[1] *= v; xyz[2] *= v; return *this; }
  
   Vector& operator/=      (const double v)
   { xyz[0] /= v; xyz[1] /= v; xyz[2] /= v; return *this; }
  

   const double& operator[] (const int v) const { return xyz[v]; }
         double& operator[] (const int v)       { return xyz[v]; }
   const double& operator() (const int v) const { return xyz[v]; }

   const double X() const { return xyz[0]; }
   const double Y() const { return xyz[1]; }
   const double Z() const { return xyz[2]; }

   const double magnitude() const 
   { return sqrt( xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2] ); }

   const Vector unitvector() const { return *this/magnitude(); }

   void normalize() 
   { double mag = magnitude(); xyz[0] /= mag; xyz[1] /= mag; xyz[2] /= mag; }

//  Comparisons

   const bool operator==         (const Vector& v) const
       { return ( xyz[0]==v.xyz[0] && xyz[1]==v.xyz[1] && xyz[2]==v.xyz[2] ); }
  
   const bool operator!=         (const Vector& v) const
       { return ( xyz[0]!=v.xyz[0] || xyz[1]!=v.xyz[1] || xyz[2]!=v.xyz[2] ); }
  
   const bool operator<          (const Vector& v) const
       { return ( magnitude() < v.magnitude() ); }
  
   const bool operator<=         (const Vector& v) const
       { return ( magnitude() <= v.magnitude() ); }
  
   const bool operator>          (const Vector& v) const
       { return ( magnitude() > v.magnitude() ); }
  
   const bool operator>=         (const Vector& v) const
       { return ( magnitude() >= v.magnitude() ); }

   // Test for parallel
   const bool operator||         (const Vector& v) const
       { return (  fabs((*this)*v) == v.magnitude()*((*this).magnitude()) ); }
  

   char *__str__() const {
       static char tmp[1024];
       std::sprintf(tmp,"Vector(%g,%g,%g)", xyz[0],xyz[1],xyz[2]);
       return tmp;
   }

   Vector rotate( const Vector& axis, const double theta ) const
   {
      double ctheta = std::cos(theta);
      double stheta = std::sin(theta);
      double va = axis*(*this);
      Vector vca = axis^(*this);
      Vector result = (*this)*ctheta + axis*va*(1.0-ctheta) + vca*stheta;
      return result;
   }

  private:
  double xyz[3];
};

template <typename T>
class Vector2
{
  public:

   Vector2(){ xy[0] = xy[1] = xy[2] = 0; }

   Vector2(const Vector2& v)
   { 
      xy[0] = v.xy[0];
      xy[1] = v.xy[1];
   }
   
   Vector2(const T a, const T b)
   {
      xy[0] = a;
      xy[1] = b;
   }

   ~Vector2(){}

   //!  Set all three components
   void set( const T vx, const T vy )
   {
      xy[0] = vx;
      xy[1] = vy;
   }

   //! Add two vector2s together
   const Vector2 operator+        (const Vector2& v) const 
   { 
      return Vector2(xy[0]+v.xy[0], xy[1]+v.xy[1]); 
   }
  
   //! Subtract one vector2 from another
   const Vector2 operator-        (const Vector2& v) const
   { 
      return Vector2(xy[0]-v.xy[0], xy[1]-v.xy[1]); 
   }

   //! Unary minus
   friend const Vector2 operator- (const Vector2& v)
   { return Vector2(-v.xy[0],-v.xy[1]); }

   //! Multiplication of a constant with a vector2
   friend const Vector2 operator* (const double w, const Vector2& v)
   { return v*w; }
	  
   //! Multiplication of a vector2 with a constant
   const Vector2 operator*        (const double v) const
   { return Vector2(xy[0]*v, xy[1]*v); }

   const Vector2 operator/        (const double v) const
   { return Vector2(xy[0]/v, xy[1]/v); }

   //! Inner product
   const double operator*        (const Vector2& v) const  
   { return (xy[0]*v.xy[0] + xy[1]*v.xy[1]); }
  
   //! cross product
   const T operator^        (const Vector2& v) const 
   { return xy[0]*v.xy[1] - xy[1]*v.xy[0]; }

   Vector2& operator=       (const Vector2& v)
   { xy[0] = v.xy[0]; xy[1] = v.xy[1]; return *this; }
  
   Vector2& operator+=      (const Vector2& v)
   { xy[0] += v.xy[0]; xy[1] += v.xy[1]; return *this; }
  
   Vector2& operator-=      (const Vector2& v)
   { xy[0] -= v.xy[0]; xy[1] -= v.xy[1]; return *this; }
  
   Vector2& operator*=      (const double v)
   { xy[0] *= v; xy[1] *= v; return *this; }
  
   Vector2& operator/=      (const double v)
   { xy[0] /= v; xy[1] /= v; return *this; }
  

   const double& operator[] (const int v) const { return xy[v]; }
         double& operator[] (const int v)       { return xy[v]; }
   const double& operator() (const int v) const { return xy[v]; }

   const double X() const { return xy[0]; }
   const double Y() const { return xy[1]; }

   const double magnitude() const 
   { return sqrt( xy[0]*xy[0] + xy[1]*xy[1] ); }

   const Vector2 unitvector2() const { return *this/magnitude(); }

   void normalize() 
   { double mag = magnitude(); xy[0] /= mag; xy[1] /= mag; }

//  Comparisons

   const bool operator==         (const Vector2& v) const
       { return ( xy[0]==v.xy[0] && xy[1]==v.xy[1] ); }
  
   const bool operator!=         (const Vector2& v) const
       { return ( xy[0]!=v.xy[0] || xy[1]!=v.xy[1] ); }
  
   const bool operator<          (const Vector2& v) const
       { return ( magnitude() < v.magnitude() ); }
  
   const bool operator<=         (const Vector2& v) const
       { return ( magnitude() <= v.magnitude() ); }
  
   const bool operator>          (const Vector2& v) const
       { return ( magnitude() > v.magnitude() ); }
  
   const bool operator>=         (const Vector2& v) const
       { return ( magnitude() >= v.magnitude() ); }

   // Test for parallel
   const bool operator||         (const Vector2& v) const
       { return (  fabs((*this)*v) == v.magnitude()*((*this).magnitude()) ); }
  

   char *__str__() const {
       static char tmp[1024];
       std::sprintf(tmp,"Vector2(%g,%g,%g)", xy[0],xy[1]);
       return tmp;
   }

//    Vector2 rotate( const Vector2& axis, const double theta ) const
//    {
//       double ctheta = std::cos(theta);
//       double stheta = std::sin(theta);
//       double va = axis*(*this);
//       Vector2 vca = axis^(*this);
//       Vector2 result = (*this)*ctheta + axis*va*(1.0-ctheta) + vca*stheta;
//       return result;
//    }

  private:
  T xy[2];
};

using vec2d = Vector2<double>;

}



#endif