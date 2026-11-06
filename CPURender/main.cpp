#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include "main.h"
#include "engine.h"
#include <iostream>
#include <chrono>
#include "Cursor.h"

//Students cannot change anything in this file


/* Main function */
int main(void) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    std::chrono::steady_clock::time_point lastFrameTime;
    double fps;
    int frameCount = 0;
    double elapsedTime = 0;

    const double target_frame = 1.0 / 60.0;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(
        "Framebuffer",
        WIDTH * 2,
        HEIGHT * 2,
        0
    );

    Cursor::SetSDLWindow(window);

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(
        window,
        NULL
    );

    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    SDL_SetRenderVSync(renderer,1);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    if (texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    Engine engine;
    engine.init();
    bool isRunning = true;
    uint32_t frame = 1;
    std::unique_ptr<FinalFrameBuffer> frameBuffer = std::make_unique<FinalFrameBuffer>();
    

    /* Frame loop */
    while (isRunning) {
        uint64_t start = SDL_GetPerformanceCounter();

        /* Poll events */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
            engine.handleEvent(event);
        }
        engine.Update();
        engine.renderFrame(*frameBuffer.get());

        /* Copy contents of the framebuffer to our SDL texture */
        SDL_UpdateTexture(
            texture,
            NULL,
            &frameBuffer.get()->colorData,
            WIDTH * sizeof(uint32_t)
        );

        /* Render texture and display the renderer */
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        uint64_t end = SDL_GetPerformanceCounter();

        double elapsed = (double)(end - start) / (double)SDL_GetPerformanceFrequency();

        /* Cap the framerate to 60 FPS (16 milliseconds each frame) */
        if (elapsed < target_frame) {
            SDL_Delay((uint32_t)((target_frame - elapsed) * 1000.0));

          
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> delta = now - lastFrameTime;
        lastFrameTime = now;

        double frameTime = delta.count();
        if (frameTime > 0) {
            fps = 1.0 / frameTime; // FPS instantané
        }

        // Calcul FPS moyen sur plusieurs frames
        elapsedTime += frameTime;
        frameCount++;

        if (elapsedTime >= 1.0) { // toutes les 1 seconde
            double avgFPS = frameCount / elapsedTime;
            std::cout << "FPS moyen : " << std::fixed  << avgFPS << "\n";
            frameCount = 0;
            elapsedTime = 0.0;
        }
        frame++;
    }

    engine.destroy();
    /* Free all the SDL resources we created */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}