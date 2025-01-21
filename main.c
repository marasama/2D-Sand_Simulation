#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEIGHT 800
#define WIDTH 900
#define SAND 1
#define WATER 2
#define PARTICLE_SIZE 5
#define PARTICLE_WIDTH WIDTH / PARTICLE_SIZE
#define PARTICLE_HEIGHT HEIGHT/ PARTICLE_SIZE
#define SAND_COLOR 0xFFFFBF00
#define WATER_COLOR 0xFF0000FF

typedef unsigned char byte;

typedef struct
{
	byte particles[PARTICLE_WIDTH][PARTICLE_HEIGHT];
	byte mode;
} Particle;

void drawGrid(SDL_Surface *screen);

void drawParticles(SDL_Surface *screen, Particle *particles);

void proccessInputs(unsigned char* running, SDL_Event* event, Particle *particles);

void simulate(Particle *particles);

void swap(byte *a, byte *b)
{
	byte tmp = *a;
	*a = *b;
	*b = tmp;
}

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

	Particle particles;

	memset(particles.particles, 0, PARTICLE_WIDTH * PARTICLE_HEIGHT);

	particles.mode = SAND;

	while (running)
	{
		while (SDL_PollEvent(&event))
			proccessInputs(&running, &event, &particles);
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

void putParticles(Particle *particles, int x, int y, int size)
{
	for (int i = x - size / 2; i < x + size / 2; i++)
	{
		for (int j = y - size / 2; j < y + size / 2; j++)
		{
			if (i >= 0 && i < PARTICLE_WIDTH && j >= 0 && j < PARTICLE_HEIGHT)
				particles->particles[i][j] = particles->mode;
		}
	}
}

void proccessInputs(unsigned char* running, SDL_Event* event, Particle *particles)
{
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
	{
		*running = 0;
	}
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_s)
	{
		particles->mode = SAND;
	}
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_w)
	{
		particles->mode = WATER;
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
			putParticles(particles, event->button.x / PARTICLE_SIZE, event->button.y / PARTICLE_SIZE, 20);
		}
	}
}

void calculateSand(Particle *particles, int i, int j)
{
	if (particles->particles[i][j] == 1 && j + 1 < PARTICLE_HEIGHT)
	{
		if (particles->particles[i][j + 1] != 1)
			swap(&particles->particles[i][j], &particles->particles[i][j + 1]);
		else if (particles->particles[i + 1][j + 1] != 1 \
				&& particles->particles[i - 1][j + 1] != 1 \
				&& particles->particles[i + 1][j] != 1)
		{
			if (rand() % 2 == 0 && i + 1 < PARTICLE_WIDTH) 
				swap(&particles->particles[i][j], &particles->particles[i + 1][j + 1]);
			else if (i - 1 >= 0) 
				swap(&particles->particles[i][j], &particles->particles[i - 1][j + 1]);
		}
		else if (particles->particles[i + 1][j + 1] != 1 && i + 1 < PARTICLE_WIDTH) 
			swap(&particles->particles[i][j], &particles->particles[i + 1][j + 1]);
		else if (particles->particles[i - 1][j + 1] != 1 && i - 1 >= 0 )
			swap(&particles->particles[i][j], &particles->particles[i - 1][j + 1]);
	}
}

void calculateWater(Particle *particles, int i, int j)
{
	if (particles->particles[i][j] == 2 && j + 1 < PARTICLE_HEIGHT)
	{
		if (particles->particles[i][j + 1] == 0)
			swap(&particles->particles[i][j], &particles->particles[i][j + 1]);
		else if (particles->particles[i][j + 1] != 0 && \
				i + 1 < PARTICLE_WIDTH && \
				particles->particles[i + 1][j] == 0 && \
				particles->particles[i - 1][j] == 0)
		{
			if (rand() % 2 == 0)
				swap(&particles->particles[i][j], &particles->particles[i + 1][j]);
			else
				swap(&particles->particles[i][j], &particles->particles[i - 1][j]);
		}
		else if (particles->particles[i + 1][j] == 0 && i + 1 < PARTICLE_WIDTH) 
			swap(&particles->particles[i][j], &particles->particles[i + 1][j]);
		else if (particles->particles[i - 1][j] == 0 && i - 1 >= 0 )
			swap(&particles->particles[i][j], &particles->particles[i - 1][j]);
	}
}

void simulate(Particle *particles)
{
	for (int i = PARTICLE_WIDTH - 1; i >= 0; i--)
	{
		for (int j =  PARTICLE_HEIGHT - 1; j >= 0; j--)
		{
			calculateWater(particles, i, j);
			calculateSand(particles, i, j);
		}
	}
}

void drawParticles(SDL_Surface *screen, Particle *particles)
{
	for (unsigned int i = 0; i < PARTICLE_WIDTH; i++)
	{
		for (unsigned int j = 0; j < PARTICLE_HEIGHT; j++)
		{
			if (particles->particles[i][j] == 1)
				AID_PutRectangle(screen, i * PARTICLE_SIZE, j * PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_SIZE, SAND_COLOR);
			if (particles->particles[i][j] == 2)
				AID_PutRectangle(screen, i * PARTICLE_SIZE, j * PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_SIZE, WATER_COLOR);
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
