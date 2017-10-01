

#include <stdbool.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "SDL_opengl.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <math.h>

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) {
	GLdouble xmin, xmax, ymin, ymax;
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif

using namespace std;

float width = 200, height = 200;
float bpp = 0;
float near = 10.0, far = 100000.0, fovy = 45.0;
float position[3] = {0,0,-40};
const float triangle[9] = {
	  0,  10, 0,  // top point
	-10, -10, 0,  // bottom left
	 10, -10, 0   // bottom right
};
float rotate_degrees  = 90;
float rotate_axis[3] = {0,1,0};

SDL_Surface* screen = nullptr;

bool IsSurfaceRGBA8888(const SDL_Surface* surface) {
	return (surface->format->Rmask == 0xFF000000 &&
			surface->format->Gmask == 0x00FF0000 &&
			surface->format->Bmask == 0x0000FF00 &&
			surface->format->Amask == 0x000000FF);
}
/*
SDL_Surface* EnsureSurfaceRGBA8888(SDL_Surface* surface) {
	// Just return if it is already RGBA8888
	if (IsSurfaceRGBA8888(surface)) {
		return surface;
	}

	// Convert the surface into a new one that is RGBA8888
	//std::cout << "Converting surface to RGBA8888 format." << std::endl;
	SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
	if (new_surface == nullptr) {
		stringstream ss;
		ss << "Failed to convert surface to RGBA8888 format" <<
		" at " << __FILE__ << ":" << __LINE__;
		throw std::runtime_error(ss.str());
	}
	SDL_FreeSurface(surface);

	// Make sure the new surface is RGBA8888
	if (! IsSurfaceRGBA8888(new_surface)) {
		stringstream ss;
		ss << "Failed to convert surface to RGBA8888 format" <<
		" at " << __FILE__ << ":" << __LINE__;
		throw std::runtime_error(ss.str());
	}
	return new_surface;
}
*/
SDL_Surface* LoadSurface(std::string file_name) {
	SDL_Surface* surface = IMG_Load(file_name.c_str());
	if (surface == nullptr) {
		stringstream ss;
		ss << "Failed to load surface \"" << file_name << "\"" <<
		" at " << __FILE__ << ":" << __LINE__;
		throw std::runtime_error(ss.str());
	}
	//surface = EnsureSurfaceRGBA8888(surface);

	return surface;
}

void render() {
	SDL_Event event;
	while ( SDL_PollEvent( &event ) ) {
		#ifdef EMSCRIPTEN
		if ( event.type == SDL_KEYDOWN || event.type == SDL_KEYUP ) {
			emscripten_cancel_main_loop();
			SDL_Quit();
		}	else if (event.type == SDL_QUIT) {
			emscripten_cancel_main_loop();
			SDL_Quit();
		}
		#endif
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(position[0], position[1], position[2]);

	{
		static Uint32 last = 0;
		static float angle = 0;

		Uint32 now = SDL_GetTicks();
		float delta = (now - last) / 1000.0f; // in seconds
		last = now;

		angle += rotate_degrees * delta;

		// c modulo operator only supports ints as arguments
		#define MOD( n, d ) (n - (d * (int) ( n / d )))
		angle = MOD( angle, 360 );

		glRotatef( angle, rotate_axis[0], rotate_axis[1], rotate_axis[2] );
	}

	glBegin(GL_TRIANGLES);
	for (int i=0; i<=6; i+=3) {
		glColor3f(i==0, i==3, i==6); // adding some color
		glVertex3fv(&triangle[i]);
	}
	glEnd();

	SDL_GL_SwapBuffers();
}

int main() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );

	screen = SDL_SetVideoMode(
		width, height, bpp,
		SDL_ANYFORMAT | SDL_OPENGL );

	glViewport(0, 0, width, height);

	glPolygonMode( GL_FRONT, GL_FILL );
	glPolygonMode( GL_BACK,  GL_LINE );

	glMatrixMode(GL_PROJECTION);
	gluPerspective(fovy,width/height,near,far);

	glMatrixMode(GL_MODELVIEW);

	// ===================
	// Texture 2
	// ===================
	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	SDL_Surface* surface = nullptr;
	try {
		//LoadSurface("awesomeface.png");
	} catch (const std::runtime_error &err) {
		//std::exception_ptr err = std::current_exception();
		cout << "!!!" << err.what() << endl;
		return 1;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(surface);
	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef EMSCRIPTEN
	emscripten_set_main_loop(render, 0, true);
#else
	while (true) {
		render();
	}
#endif
	return 0;
}
