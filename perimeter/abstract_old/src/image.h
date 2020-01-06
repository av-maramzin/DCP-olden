#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

enum class ElementColor {
    colorless,
    black,
    white
};

class Image;
class RingImage;
class Point;
class GridElement;
class Grid;

class Point {

    public:
        Point() : x(0), y(0) {}
        Point(double x, double y) {
            this->x = x;
            this->y = y;
        }

        Point(Point& point) {
            this->x = point.x;
            this->y = point.y;
        }
 
        Point(const Point& point) {
            this->x = point.x;
            this->y = point.y;
        }
       
        Point& operator=(Point& point) {
            this->x = point.x;
            this->y = point.y;
            return *this;
        }

        double x;
        double y;
};

class GridElement {
    
    public:
        GridElement() : 
            center(0.0, 0.0), elementSize(0.0),
            nw(center.x-elementSize/2, center.y+elementSize/2), ne(center.x+elementSize/2, center.y+elementSize/2),
            sw(center.x-elementSize/2, center.y-elementSize/2), se(center.x+elementSize/2, center.y-elementSize/2),
            color(ElementColor::colorless) {}
 
        GridElement(Point center, double size) : 
            center(center), elementSize(size),
            nw(center.x-elementSize/2, center.y+elementSize/2), ne(center.x+elementSize/2, center.y+elementSize/2),
            sw(center.x-elementSize/2, center.y-elementSize/2), se(center.x+elementSize/2, center.y-elementSize/2),
            color(ElementColor::colorless) {}

        GridElement& operator=(GridElement& elem) {
            this->center = elem.center;
            this->nw = elem.nw;
            this->ne = elem.ne;
            this->sw = elem.sw;
            this->se = elem.se;
            this->color = elem.color;
            this->elementSize = elem.elementSize;
        }
   
        Point center;
        // corner points
        Point nw; // northwest
        Point ne; // northeast
        Point sw; // southwest
        Point se; // southeast
        
        ElementColor color;
        
        double elementSize;
};

class Grid {

    public:
        Grid(const int size, const double elementSize); 

        void mapImage(Image& image);

        const int size;
        double elementSize;

        std::vector<GridElement> grid;
};

class Image {
    
    public:
        Image() : mapped(false) {}
        virtual ~Image() {}
        virtual bool inside(const Point point) = 0;

        int perimeter();
        
        void print();

        void map(Grid* grid) { 
            this->mapped = true; 
            this->grid = grid;    
        }

        bool isMapped() { return mapped; }

    protected:
        bool mapped;
        Grid* grid;
};

class RingImage : public Image {

    public:
        RingImage(int innerRadius, int outerRadius) {
            this->r = innerRadius;
            this->R = outerRadius;
        }

        ~RingImage() {}

        bool inside(const Point point);

        int r;
        int R;
};

#endif // #ifndef IMAGE_H
