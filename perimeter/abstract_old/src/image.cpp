#include "image.h"

#include <cstdlib>
#include <iostream>

Grid::Grid(const int size, const double elementSize) 
    : size(size), elementSize(elementSize) 
{
    grid.reserve(size*size);
            
    double x;
    double y = (size/2)*elementSize - elementSize/2;
    for (int i = 0; i < size; i++) {
        y -= elementSize;
        x = (-size/2)*elementSize + elementSize/2;
        for (int j = 0; j < size; j++) {
            x += elementSize;
                  
            Point center(x,y);
            GridElement elem(center, elementSize);
            grid.push_back(elem);
        }
    }

}

void Grid::mapImage(Image& image) {

#pragma omp parallel for
    for(int i = 0; i < grid.size(); i++) {
        GridElement& elem = grid[i];
        
        if ( !image.inside(elem.nw) && !image.inside(elem.ne) &&
             !image.inside(elem.sw) && !image.inside(elem.se) ) {
            elem.color = ElementColor::white; 
        } else {
            elem.color = ElementColor::black; 
        }
    }
}

bool RingImage::inside(const Point point) {
    double euclid = point.x*point.x + point.y*point.y;

    if ( euclid > R*R || euclid < r*r ) { 
        return false;
    } else {
        return true;
    }
}

int Image::perimeter() {
    if (this->isMapped()) {
        Grid& g = *this->grid;    
        double perim = 0;
        static int flips = 0;
       
        int row;
#pragma omp parallel for private(row) schedule(dynamic) reduction(+:perim)
        for(row = 0; row < g.size; row++) {
            for (int column = 0; column < g.size; column++) {
                if ( g.grid[row*g.size + column].color != g.grid[row*g.size + column+1].color ) {
                    // change of the color -> current element is on 
                    // the perimeter of the image
                    perim += g.elementSize;
                    flips++;
                }
            }
        }
    
        std::cout << "Flips: " << flips << std::endl;
        return (int)perim;

    } else {
        std::cerr << "Image::perimeter() : Image must be mapped on the grid!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Image::print() {
    if (this->isMapped()) {
        Grid& g = *this->grid;    
        for(int row = 0; row < g.size; row++) {
            for (int column = 0; column < g.size; column++) {
                if ( g.grid[row*g.size + column].color == ElementColor::black ) {
                    std::cout << "#";
                } else if ( g.grid[row*g.size + column].color == ElementColor::white ) {
                    std::cout << ".";
                }
            }
            std::cout << std::endl;
        }
    } else {
        std::cerr << "Image::print() : Image must be mapped on the grid!" << std::endl;
        exit(EXIT_FAILURE);
    }
}
