#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEIGHT 800
#define WIDTH 900
#define PARTICLE_SIZE 10
#define PARTICLE_WIDTH WIDTH / PARTICLE_SIZE
#define PARTICLE_HEIGHT HEIGHT/ PARTICLE_SIZE
#define PARTICLE_COLOR 0xFFFFBF00

typedef unsigned char byte;

void drawGrid(SDL_Surface *screen);

void drawParticles(SDL_Surface *screen, byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT]);

void proccessInputs(unsigned char* running, SDL_Event* event, byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT]);

void simulate(byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT]);

void AID_PutPixel(SDL_Surface *surface, int x, int y, uint32_t color)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
	{
		uint32_t *pixel = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch + x * 4);
		*pixel = color;
	}

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void AID_PutRectangle(SDL_Surface *surface, int x, int y, int w, int h, uint32_t color)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	for (int i = x; i < x + w; i++)
	{
		for (int j = y; j < y + h; j++)
		{
			AID_PutPixel(surface, i, j, color);
		}
	}
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

int main()
{
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	if (window == NULL && screen == NULL)
	{
		printf("Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Event event;

	unsigned char running = 1;

	byte particles[PARTICLE_WIDTH][PARTICLE_HEIGHT];

	memset(particles, 0, PARTICLE_WIDTH * PARTICLE_HEIGHT);

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			proccessInputs(&running, &event, &particles);
		}
		AID_PutRectangle(screen, 0, 0, WIDTH, HEIGHT, 0xFF000000);
		simulate(&particles);
		drawParticles(screen, &particles);
		//drawGrid(screen);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(10);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return (0);
}

void putParticles(byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT], int x, int y, int size)
{
	for (int i = x - size / 2; i < x + size / 2; i++)
	{
		for (int j = y - size / 2; j < y + size / 2; j++)
		{
			if (i >= 0 && i < PARTICLE_WIDTH && j >= 0 && j < PARTICLE_HEIGHT)
				(*particles)[i][j] = 1;
		}
	}
}

void proccessInputs(unsigned char* running, SDL_Event* event, byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT])
{
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
	{
		*running = 0;
	}
	if (event->type == SDL_QUIT)
	{
		*running = 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN \
			&& event->button.button == SDL_BUTTON_LEFT)
	{
		if (event->button.x >= 0 && event->button.x < WIDTH && event->button.y >= 0 && event->button.y < HEIGHT)
		{
			putParticles(particles, event->button.x / PARTICLE_SIZE, event->button.y / PARTICLE_SIZE, 10);
		}
	}
}

void calculateParticles(byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT], unsigned int i, unsigned int j)
{
	if ((*particles)[i][j] == 1 && j + 1 < PARTICLE_HEIGHT)
	{
		if ((*particles)[i][j + 1] == 0)
		{
			(*particles)[i][j] = 0;
			(*particles)[i][j + 1] = 1;
		}
		else if ((*particles)[i + 1][j + 1] == 0 \
				&& (*particles)[i - 1][j + 1] == 0 \
				&& (*particles)[i + 1][j] == 0)
		{
			(*particles)[i][j] = 0;
			if (rand() % 2 == 0 && i + 1 < PARTICLE_WIDTH) 
				(*particles)[i + 1][j + 1] = 1;
			else if (i - 1 >= 0) 
				(*particles)[i - 1][j + 1] = 1;
		}
		else if ((*particles)[i + 1][j + 1] == 0 && (*particles)[i + 1][j] == 0 && i + 1 < PARTICLE_WIDTH) 
		{
			(*particles)[i][j] = 0;
			(*particles)[i + 1][j + 1] = 1;
		}
		else if ((*particles)[i - 1][j + 1] == 0 && (*particles)[i - 1][j] == 0 && i - 1 >= 0 )
		{
			(*particles)[i][j] = 0;
			(*particles)[i - 1][j + 1] = 1;
		}

	}
}

void calculateWater(byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT], unsigned int i, unsigned int j)
{
	if ((*particles)[i][j] == 1 && j + 1 < PARTICLE_HEIGHT)
	{
		if ((*particles)[i][j + 1] == 0)
		{
			(*particles)[i][j] = 0;
			(*particles)[i][j + 1] = 1;
		}
		else if ((*particles)[i + 1][j + 1] == 0 \
				&& (*particles)[i - 1][j + 1] == 0 \
				&& (*particles)[i + 1][j] == 0)
		{
			(*particles)[i][j] = 0;
			if (rand() % 2 == 0 && i + 1 < PARTICLE_WIDTH) 
				(*particles)[i + 1][j + 1] = 1;
			else if (i - 1 >= 0) 
				(*particles)[i - 1][j + 1] = 1;
		}
		else if ((*particles)[i + 1][j + 1] == 0 && (*particles)[i + 1][j] == 0 && i + 1 < PARTICLE_WIDTH) 
		{
			(*particles)[i][j] = 0;
			(*particles)[i + 1][j + 1] = 1;
		}
		else if ((*particles)[i - 1][j + 1] == 0 && (*particles)[i - 1][j] == 0 && i - 1 >= 0 )
		{
			(*particles)[i][j] = 0;
			(*particles)[i - 1][j + 1] = 1;
		}

	}
}

void simulate(byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT])
{
	for (int i = PARTICLE_WIDTH - 1; i >= 0; i--)
	{
		for (int j =  PARTICLE_HEIGHT - 1; j >= 0; j--)
		{
			calculateParticles(particles, i, j);
			calculateWater(particles, i, j);
		}
	}
}

void drawParticles(SDL_Surface *screen, byte (*particles)[PARTICLE_WIDTH][PARTICLE_HEIGHT])
{
	for (unsigned int i = 0; i < PARTICLE_WIDTH; i++)
	{
		for (unsigned int j = 0; j < PARTICLE_HEIGHT; j++)
		{
			if ((*particles)[i][j] == 1)
				AID_PutRectangle(screen, i * PARTICLE_SIZE, j * PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_COLOR);
		}
	}
}

void drawGrid(SDL_Surface *screen)
{
	for (int i = 0; i < WIDTH; i += PARTICLE_SIZE)
	{
		AID_PutRectangle(screen, i, 0, 1, HEIGHT, 0xFF0000FF);
	}
	for (int j = 0; j < HEIGHT; j += PARTICLE_SIZE)
	{
		AID_PutRectangle(screen, 0, j, WIDTH, 1, 0xFF0000FF);
	}
}
