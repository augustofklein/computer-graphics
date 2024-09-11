#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stack>
#include <sstream>
#include <unistd.h>
#include <list>

using namespace std;

// PRAGMA
float rfPartOfNumber(float x);
void swap(int* a , int*b);
void drawPixel( int x , int y , float brightness);


// SDL stuff
SDL_Window* pWindow = 0;
SDL_Renderer* pRenderer = 0;
SDL_Surface * window_surface;

// draws a pixel on screen of given brightness
// 0<=brightness<=1. We can use your own library
// to draw on screen
void drawPixel( int x , int y , float brightness)
{
    //printf("(%d,%d)\n",x,y);
	int c = 255*brightness;
	SDL_SetRenderDrawColor(pRenderer, c, c, c, 255);
	SDL_RenderDrawPoint(pRenderer, x, y);
}

void drawPixel( int x , int y , int r, int g, int b, float brightness)
{
	int r1 = r*brightness;
	int g1 = g*brightness;
	int b1 = b*brightness;
	SDL_SetRenderDrawColor(pRenderer, r1, g1, b1, brightness);
	SDL_RenderDrawPoint(pRenderer, x, y);
}



void setPixel(int x, int y, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(pRenderer, r, g, b, a);
	SDL_RenderDrawPoint(pRenderer, x, y);
}

void setPixel(int x, int y, int r, int g, int b)
{
	setPixel(x, y, r, g, b, 255);
}

Uint32 RGB(int r, int g, int b) {
    return SDL_MapRGBA(window_surface->format, r, g, b, 255);
}

Uint8 getColorComponent( Uint32 pixel, char component ) {

    Uint32 mask;

    switch(component) {
        case 'b' :
        case 'B' :
            mask = RGB(0,0,255);
            pixel = pixel & mask;
            break;
        case 'r' :
        case 'R' :
            mask = RGB(255,0,0);
            pixel = pixel & mask;
            pixel = pixel >> 16;
            break;
        case 'g' :
        case 'G' :
            mask = RGB(0,255,0);
            pixel = pixel & mask;
            pixel = pixel >> 8;
            break;
    }
    return (Uint8) pixel;
}

void setPixel(int x, int y, Uint32 color)
{
    int r = getColorComponent(color, 'r');
    int g = getColorComponent(color, 'g');
    int b = getColorComponent(color, 'b');
    SDL_SetRenderDrawColor(pRenderer, r, g, b, 255);
	SDL_RenderDrawPoint(pRenderer, x, y);
}

Uint32 getPixel(int x, int y)
{
    int bpp = window_surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *) window_surface->pixels + y * window_surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

            case 4:
                return *(Uint32 *)p;
                break;

            default:
                return 0;       /* shouldn't happen, but avoids warnings */
      }
}

void bresenham(int x1, int y1, int x2, int y2, int r, int g, int b)
{
    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;

    dx1=fabs(dx);
    dy1=fabs(dy);

    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        setPixel(x,y,r,g,b);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
            setPixel(x,y,r,g,b);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        setPixel(x,y,r,g,b);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
            setPixel(x,y,r,g,b);
        }
    }
}

void bresenham(int x1, int y1, int x2, int y2)
{
    bresenham(x1, y1, x2, y2, 0, 0, 0);
}



// retorna o valor absoluto de um n�mero
float absolute(float x )
{
	if (x < 0) return -x;
	else return x;
}

// retorna a parte inteira de um n�mero
// de ponto flutuante
int iPartOfNumber(float x)
{
	return (int)x;
}

// arredonda um n�mero
int roundNumber(float x)
{
	return iPartOfNumber(x + 0.5) ;
}

// retorna a parte decimal de um n�mero
float fPartOfNumber(float x)
{
	if (x>0) return x - iPartOfNumber(x);
	else return x - (iPartOfNumber(x)+1);

}



void drawWuLine(int x0 , int y0 , int x1 , int y1)
{
	int steep = absolute(y1 - y0) > absolute(x1 - x0) ;

	// inverte-se as coordenadas             se a inclina��o > 1
	// ou se o desenho � da direita para a esquerda
	if (steep)
	{
		swap(&x0 , &y0);
		swap(&x1 , &y1);
	}
	if (x0 > x1)
	{
		swap(&x0 ,&x1);
		swap(&y0 ,&y1);
	}

	//calcula-se a inclina��o da reta (slope)
	float dx = x1-x0;
	float dy = y1-y0;
	float gradient = dy/dx;
	if (dx == 0.0)
		gradient = 1;

	int xpxl1 = x0;
	int xpxl2 = x1;
	float intersectY = y0;

	// la�o principal
	if (steep)
	{
		int x;
		for (x = xpxl1 ; x <=xpxl2 ; x++)
		{
			// a cobertura do pixel � determinada pela parte
			// decimal da coordenada
			drawPixel(iPartOfNumber(intersectY), x,
						rfPartOfNumber(intersectY));
			drawPixel(iPartOfNumber(intersectY)-1, x,
						fPartOfNumber(intersectY));
			intersectY += gradient;
		}
	}
	else
	{
		int x;
		for (x = xpxl1 ; x <=xpxl2 ; x++)
		{
			// a cobertura do pixel � determinada pela parte
			// decimal da coordenada
			drawPixel(x, iPartOfNumber(intersectY),
						rfPartOfNumber(intersectY));
			drawPixel(x, iPartOfNumber(intersectY)-1,
						fPartOfNumber(intersectY));
			intersectY += gradient;
		}
	}

}

// troca dois n�meros
void swap(int* a , int*b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

// returns 1 - a parte decimal de um n�mero
float rfPartOfNumber(float x)
{
	return 1 - fPartOfNumber(x);
}

void drawWuLine(int x0 , int y0 , int x1 , int y1, int r, int g, int b)
{
	int steep = absolute(y1 - y0) > absolute(x1 - x0) ;

	// inverte-se as coordenadas             se a inclina��o > 1
	// ou se o desenho � da direita para a esquerda
	if (steep)
	{
		swap(&x0 , &y0);
		swap(&x1 , &y1);
	}
	if (x0 > x1)
	{
		swap(&x0 ,&x1);
		swap(&y0 ,&y1);
	}

	//calcula-se a inclina��o da reta (slope)
	float dx = x1-x0;
	float dy = y1-y0;
	float gradient = dy/dx;
	if (dx == 0.0)
		gradient = 1;

	int xpxl1 = x0;
	int xpxl2 = x1;
	float intersectY = y0;

	// la�o principal
	if (steep)
	{
		int x;
		for (x = xpxl1 ; x <=xpxl2 ; x++)
		{
			// a cobertura do pixel � determinada pela parte
			// decimal da coordenada
			drawPixel(iPartOfNumber(intersectY), x, r, g, b,
						rfPartOfNumber(intersectY));
			drawPixel(iPartOfNumber(intersectY)-1, x, r, g, b,
						fPartOfNumber(intersectY));
			intersectY += gradient;
		}
	}
	else
	{
		int x;
		for (x = xpxl1 ; x <=xpxl2 ; x++)
		{
			// a cobertura do pixel � determinada pela parte
			// decimal da coordenada
			drawPixel(x, iPartOfNumber(intersectY), r, g, b,
						rfPartOfNumber(intersectY));
			drawPixel(x, iPartOfNumber(intersectY)-1, r, g, b,
						fPartOfNumber(intersectY));
			intersectY += gradient;
		}
	}

}

void drawWuLine(int x0, int y0, int x1, int y1, Uint32 clrLine )
{
    /* Make sure the line runs top to bottom */
    if (y0 > y1)
    {
        int aux = y0; y0 = y1; y1 = aux;
        aux = x0; x0 = x1; x1 = aux;
    }

    /* Draw the initial pixel, which is always exactly intersected by
    the line and so needs no weighting */
    setPixel( x0, y0, clrLine );

    int xDir, deltaX = x1 - x0;
    if( deltaX >= 0 )
    {
        xDir = 1;
    }
    else
    {
        xDir   = -1;
        deltaX = 0 - deltaX; /* make deltaX positive */
    }

    /* Special-case horizontal, vertical, and diagonal lines, which
    require no weighting because they go right through the center of
    every pixel */
    int deltaY = y1 - y0;
    if (deltaY == 0)
    {
        /* Horizontal line */
        while (deltaX-- != 0)
        {
            x0 += xDir;
            setPixel( x0, y0, clrLine );
        }
        return;
    }
    if (deltaX == 0)
    {
        /* Vertical line */
        do
        {
            y0++;
            setPixel( x0, y0, clrLine );
        } while (--deltaY != 0);
        return;
    }

    if (deltaX == deltaY)
    {
        /* Diagonal line */
        do
        {
            x0 += xDir;
            y0++;
            setPixel( x0, y0, clrLine );
        } while (--deltaY != 0);
        return;
    }

    unsigned short errorAdj;
    unsigned short errorAccaux, weighting;

    /* Line is not horizontal, diagonal, or vertical */
    unsigned short errorAcc = 0;  /* initialize the line error accumulator to 0 */

    Uint32 rl = getColorComponent( clrLine, 'r' );
    Uint32 gl = getColorComponent( clrLine, 'g' );
    Uint32 bl = getColorComponent( clrLine, 'b' );
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;

    /* Is this an X-major or Y-major line? */
    if (deltaY > deltaX)
    {
    /* Y-major line; calculate 16-bit fixed-point fractional part of a
    pixel that X advances each time Y advances 1 pixel, truncating the
        result so that we won't overrun the endpoint along the X axis */
        errorAdj = ((unsigned long) deltaX << 16) / (unsigned long) deltaY;
        /* Draw all pixels other than the first and last */
        while (--deltaY) {
            errorAccaux = errorAcc;   /* remember currrent accumulated error */
            errorAcc += errorAdj;      /* calculate error for next pixel */
            if (errorAcc <= errorAccaux) {
                /* The error accumulator turned over, so advance the X coord */
                x0 += xDir;
            }
            y0++; /* Y-major, so always advance Y */
                  /* The IntensityBits most significant bits of errorAcc give us the
                  intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
            weighting = errorAcc >> 8;
            /*
            ASSERT( weighting < 256 );
            ASSERT( ( weighting ^ 255 ) < 256 );
            */
            Uint32 clrBackGround = getPixel(x0, y0 );
            //clrBackGround = RGB(255, 255, 255);
            Uint8 rb = getColorComponent( clrBackGround, 'r' );
            Uint8 gb = getColorComponent( clrBackGround, 'g' );
            Uint8 bb = getColorComponent( clrBackGround, 'b' );
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
            Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
            Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0, y0, RGB( rr, gr, br ) );

            clrBackGround = getPixel(x0 + xDir, y0 );
            //clrBackGround = RGB(255, 255, 255);
            rb = getColorComponent( clrBackGround, 'r' );
            gb = getColorComponent( clrBackGround, 'g' );
            bb = getColorComponent( clrBackGround, 'b' );
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
            gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
            br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0 + xDir, y0, RGB( rr, gr, br ) );
        }
        /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
        setPixel( x1, y1, clrLine );
        return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
    pixel that Y advances each time X advances 1 pixel, truncating the
    result to avoid overrunning the endpoint along the X axis */
    errorAdj = ((unsigned long) deltaY << 16) / (unsigned long) deltaX;
    /* Draw all pixels other than the first and last */
    while (--deltaX) {
        errorAccaux = errorAcc;   /* remember currrent accumulated error */
        errorAcc += errorAdj;      /* calculate error for next pixel */
        if (errorAcc <= errorAccaux) {
            /* The error accumulator turned over, so advance the Y coord */
            y0++;
        }
        x0 += xDir; /* X-major, so always advance X */
                    /* The IntensityBits most significant bits of errorAcc give us the
                    intensity weighting for this pixel, and the complement of the
        weighting for the paired pixel */
        weighting = errorAcc >> 8;
        /*
        ASSERT( weighting < 256 );
        ASSERT( ( weighting ^ 255 ) < 256 );
        */
        Uint32 clrBackGround = getPixel(x0, y0 );
        //clrBackGround = RGB(255, 255, 255);
        Uint8 rb = getColorComponent( clrBackGround, 'r' );
        Uint8 gb = getColorComponent( clrBackGround, 'g' );
        Uint8 bb = getColorComponent( clrBackGround, 'b' );
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
        Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
        Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0, RGB( rr, gr, br ) );

        clrBackGround = getPixel(x0, y0 + 1 );
        //clrBackGround = RGB(255, 255, 255);
        rb = getColorComponent( clrBackGround, 'r' );
        gb = getColorComponent( clrBackGround, 'g' );
        bb = getColorComponent( clrBackGround, 'b' );
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
        gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
        br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0 + 1, RGB( rr, gr, br ) );
    }

    /* Draw the final pixel, which is always exactly intersected by the line
    and so needs no weighting */
    setPixel( x1, y1, clrLine );
}

void rectangle(int x1, int y1, int x2, int y2)
{

  int i;

  for(i = x1; i <x2; i++)
  {
    setPixel(i, y1, 0, 0, 0);
    setPixel(i, y2, 0, 0, 0);
  }

  for(i = y1; i <y2; i++)
  {
    setPixel(x1, i, 0, 0, 0);
    setPixel(x2, i, 0, 0, 0);
  }

}

void rectangle(int x1, int y1, int x2, int y2, int r, int g, int b)
{

  int i, j;

  for(i = x1; i <x2; i++)
  {
    for(j = y1; j <y2; j++)
    {
        setPixel(i, j, r, g, b);
    }
  }

  rectangle(x1,y1,x2,y2);
}

void rectangle(int x1, int y1, int x2, int y2, Uint32 color)
{

  int i, j;

  for(i = x1; i <x2; i++)
  {
    for(j = y1; j <y2; j++)
    {
        setPixel(i, j, color);
    }
  }
}

//Dimitrios Matthes and Vasileios Drakopoulos (2019)
void drawClippedLineMD(float xmin, float ymin, float xmax, float ymax,
    float x1, float y1, float x2, float y2, Uint32 color){

    float x[2],y[2];
    int  i;

    if(!(x1<xmin && x2<xmin) && !(x1>xmax && x2>xmax)) {
        if(!(y1<ymin && y2<ymin) && !(y1>ymax && y2>ymax)) {

            x[0] = x1;
            y[0] = y1;
            x[1] = x2;
            y[1] = y2;

            for(i = 0; i < 2; i++){
                if(x[i]<xmin){
                    x[i]=xmin;
                    y[i]=((y2-y1)/(x2-x1))*(xmin-x1)+y1;
                } else if(x[i]>xmax){
                    x[i]=xmax;
                    y[i]=((y2-y1)/(x2-x1))*(xmax-x1)+y1;
                }
                if(y[i]<ymin){
                    y[i]=ymin;
                    x[i]=((x2-x1)/(y2-y1))*(ymin-y1)+x1;
                } else if(y[i]>ymax){
                    y[i]=ymax;
                    x[i]=((x2-x1)/(y2-y1))*(ymax-y1)+x1;
                }

            }
            if(!(x[0]<xmin && x[1]<xmin) && !(x[0]>xmax && x[1]>xmax)) {
                drawWuLine(x[0],y[0],x[1],y[1], color);
            }
        }
    }
}

typedef struct {int x, y;} Point;

Point getPoint(int x, int y)
{
    Point p;
    p.x = x;
    p.y = y;
    return p;
}
/*
void drawPolygon(list<Point> pontos, Uint32 cor)
{
    Point primeiro = pontos.front();
    Point anterior = pontos.front();
    Point atual;

    int i = 0;
    for(Point p : pontos) {
        if(i>0){
            atual = p;
            drawWuLine(anterior.x, anterior.y, atual.x, atual.y, RGB(52, 235, 103));
            anterior = atual;
        }
        i++;
    }
    drawWuLine(atual.x, atual.y, primeiro.x, primeiro.y, RGB(52, 235, 103));
}
*/
void display()
{
    // DIAGONAL DE BAIXO PARA CIMA E DIREITA PARA ESQUERDA
    //bresenham(600, 400, 10, 10);

    // DIAGONAL DE BAIXO PARA CIMA E DIREITA PARA ESQUERDA - Com anti-Aliasing
    drawWuLine(400, 430, 10, 40);
    drawWuLine(50, 20, 500, 300);
    drawWuLine(250, 150, 250, 250);

    // desenha ret�ngulos
    rectangle(220, 200, 420, 280);

    //desenha ret�ngulo preenchido
    //rectangle (100, 200, 200, 400, RGB(0,255,255));

    //drawClippedLineMD(220, 200, 420, 280, 400, 430, 10, 40, RGB(52, 235, 103));
    //drawClippedLineMD(220, 200, 420, 280, 50, 20, 500, 300, RGB(52, 235, 103));
    //drawClippedLineMD(220, 200, 420, 280, 250, 150, 250, 250, RGB(52, 235, 103));

    //TODO: CHAMAR A FUNÇÃO drawPolygon PARA DESENHAR OS POLIGONOS DA ATIVIDADE
    //      DE NÚMERO 2.

}

// Driver code
int main(int argc, char* args[])
{

	SDL_Event event;

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		pWindow = SDL_CreateWindow("SDL_Lines",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
														640, 480,
												SDL_WINDOW_SHOWN);

		// if the window creation succeeded create our renderer
		if (pWindow != 0) {
			pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
			window_surface = SDL_GetWindowSurface(pWindow);
        }

	}
	else
		return 1; // sdl could not initialize

	while (1)
	{
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;

		// Sets background color to white
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
		SDL_RenderClear(pRenderer);

        display();

		// show the window
		SDL_RenderPresent(pRenderer);
	}

	// clean up SDL
	SDL_Quit();
	return 0;
}

