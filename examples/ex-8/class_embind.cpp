#include <emscripten/bind.h>
#include <iostream>

constexpr double PI = 3.14159265358979323846;

enum class ShapeType {
  Circle,
  Rectangle,
};

class Shape {
 public:
  Shape(ShapeType type) : m_Type(type) {}

  virtual ~Shape() {
    std::cout << "Shape destructor" << std::endl;
  }

  virtual double GetArea() const = 0;
  ShapeType GetType() const { return m_Type; }

 private:
  ShapeType m_Type;
};

class Circle : public Shape {
 public:
  static Shape* Create(double radius) {
    return new Circle(radius);
  }
  
  virtual ~Circle() {
    std::cout << "Circle destructor" << std::endl;
  }

  double GetArea() const override { return PI * m_Radius * m_Radius; }

 private:
  double m_Radius;

  Circle(double radius)
    : Shape(ShapeType::Circle), m_Radius(radius) {}
};

class Rectangle : public Shape {
 public:
  static Shape* Create(double width, double height) {
    return new Rectangle(width, height);
  }

  virtual ~Rectangle() {
    std::cout << "Rectangle destructor" << std::endl;
  }

  double GetArea() const override { return m_Width * m_Height; }

 private:
  double m_Width;
  double m_Height;

  Rectangle(double width, double height)
    : Shape(ShapeType::Rectangle), m_Width(width), m_Height(height) {}
};

EMSCRIPTEN_BINDINGS(shape_module) {
  emscripten::enum_<ShapeType>("ShapeType")
    .value("Circle", ShapeType::Circle)
    .value("Rectangle", ShapeType::Rectangle);
  emscripten::class_<Shape>("Shape")
    .function("getArea", &Shape::GetArea)
    .function("getType", &Shape::GetType);
  emscripten::function("createCircle", &Circle::Create, emscripten::allow_raw_pointers());
  emscripten::function("createRectangle", &Rectangle::Create, emscripten::allow_raw_pointers());
}