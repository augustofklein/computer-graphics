#define SDL_MAIN_HANDLED
#include<SDL2/SDL.h>
#include<stdio.h>
#include <unistd.h>

#include<Context.h>
#include<Color.h>
#include<Point.h>
#include<Line.h>


// SDL stuff
SDL_Window* pWindow = nullptr;
SDL_Renderer* pRenderer = nullptr;
SDL_Surface * window_surface = nullptr;

int offset = 10;

void display()
{

    Color color = Color(255,0,0);

    Point p1 = Point(600-offset,400);
    Point p2 = Point(10+offset,10);

    Line l1 = Line(p1, p2, color);
    l1.draw();

    Point p3 = Point(600-offset,430);
    Point p4 = Point(10+offset,40);

    offset += 10;

    if(offset > 500) {
        offset = 0;
    }

    Line l2 = Line(p3, p4, color, 1);
    l2.draw();

}

void clear() {

    Line l = Line();
    SDL_Surface * window_surface = Context::getInstance()->getWindowSurface();
    for(int x = 0;x < window_surface->w; x++){
        for(int y = 0;y < window_surface->h; y++) {
            l.setPixel(x, y, 255, 255, 255);
        }
    }

}

// Driver code
int main(int argc, char* args[])
{

	SDL_Event event;

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		pWindow = SDL_CreateWindow("SDL_Classes",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
														640, 480,
												SDL_WINDOW_SHOWN);

		// if the window creation succeeded create our renderer
		if (pWindow != 0) {
			pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
			window_surface = SDL_GetWindowSurface(pWindow);

            // Inicializa o contexto gráfico da aplicação
			Context * context = Context::getInstance();
			context->setRenderer(pRenderer);
			context->setWindowSurface(window_surface);

        }

	}
	else
		return 1; // sdl could not initialize

	while (1)
	{

        // Limpa a tela
        clear();

        // Realiza o desenho
        display();

        // Aguarda instantes
        usleep(100000);

		// Verifica se foi mandado fechar a janela
		while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

        }

		// Atualiza a tela
		SDL_UpdateWindowSurface(pWindow);
	}

	// clean up SDL
	SDL_Quit();
	return 0;
}
