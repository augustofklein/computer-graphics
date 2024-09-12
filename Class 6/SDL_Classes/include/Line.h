#ifndef LINE_H
#define LINE_H
#include <Point.h>
#include <Color.h>
#include<SDL2/SDL.h>

class Line
{
    public:
        Line();
        Line(Point start, Point end, Color color);
        Line(Point start, Point end, Color color, int antialias);
        void setPixel(int x, int y, int r, int g, int b);
        void draw();
        virtual ~Line();

    protected:

    private:
        Point start;
        Point end;
        Color color;
        int antialias = 0;
        void setPixel(int x, int y, int r, int g, int b, int a);
        void setPixel(int x, int y, Color color);
        void setPixel(int x, int y, Uint32 cor);
        Uint32 getPixel(int x, int y);
        void bresenham(int x1, int y1, int x2, int y2, int r, int g, int b);
        void drawWuLine(int x0, int y0, int x1, int y1, Color color);

};

#endif // LINE_H
