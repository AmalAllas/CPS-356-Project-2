//
// Created by strinejc on 10/22/2026.
//

#include "ProblemTwo.h"

#include <stdio.h>
#include <stdlib.h>

void problemTwo() {
    struct App *app = (struct App *)malloc(sizeof(struct App));


    printf("\nEnter the number of cycles you'd like to run: ");
    scanf("%d", &app->cycles);

    while (app->cycles <= 0) {
        printf("Input must be a positive integer");
        printf("\nEnter the number of cycles you'd like to run: ");
        scanf("%d", &app->cycles);
    }
    setUp(app);

    printCycles(app);

    performCycles(app);

    printCycles(app);

}

void setUp(struct App *app) {

    app->headFork = NULL;
    app->tailFork = NULL;

    sem_init(&app->state, 0, 1);

    initializeForks(app);
    initializePhilosophers(app);

}

void initializeForks(struct App *app) {
    struct Fork *lastFork = NULL;
    for (int i = 0; i < 5; i++) {
        struct Fork *newFork = (struct Fork *) malloc(sizeof(struct Fork));

        newFork->position = i + 1;
        newFork->next = NULL;
        newFork->prev = lastFork;
        newFork->state = AVAILABLE;

        sem_init(&newFork->semaphore, 0, 1);

        if (lastFork == NULL) {
            app->headFork = newFork;
        } else {
            lastFork->next = newFork;
        }

        lastFork = newFork;
    }

    app->tailFork = lastFork;
    app->tailFork->next = app->headFork;
    app->headFork->prev = app->tailFork;
}

void initializePhilosophers(struct App *app) {
    struct Fork *fork = app->headFork;

    for (int i = 0; i < 5; i++) {
        struct Philosopher *newPhilosopher = (struct Philosopher *) malloc(sizeof(struct Philosopher));

        newPhilosopher->position = i + 1;
        newPhilosopher->state = THINKING;
        newPhilosopher->goalCycles = app->cycles;
        newPhilosopher->completedCycles = 0;
        newPhilosopher->isCompleted = 0;

        newPhilosopher->leftFork = fork->prev;
        newPhilosopher->rightFork = fork;

        app->philosophers[i] = newPhilosopher;

        fork = fork->next;

        newPhilosopher->app = app;
    }

}

void *philosopherCycles(void *arg) {
    struct Philosopher *p = arg;
    struct App *app = p->app;

    for (int i = 0; i < p->goalCycles; i++) {
        sem_wait(&app->state);
        p->state = THINKING;
        sem_post(&app->state);

        sem_wait(&app->state);
        p->state = HUNGRY;
        sem_post(&app->state);

        getForks(p);

        sem_wait(&app->state);
        p->state = EATING;
        printCycles(app);
        sem_post(&app->state);

        freeForks(p);
    }

    return NULL;
}

void performCycles(struct App *app) {
    for (int i = 0; i < 5; i++) {
        struct Philosopher *p = app->philosophers[i];
        pthread_create(&p->thread, NULL, philosopherCycles, (void *)p);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(app->philosophers[i]->thread, NULL);
    }
}

void getForks(struct Philosopher *p) {
    if (p->position % 2 == 0) {
        // Even philosophers: left first
        sem_wait(&p->leftFork->semaphore);
        sem_wait(&p->rightFork->semaphore);
    } else {
        // Odd philosophers: right first
        sem_wait(&p->rightFork->semaphore);
        sem_wait(&p->leftFork->semaphore);
    }

    sem_wait(&p->app->state);

    p->leftFork->state = UNAVAILABLE;
    p->rightFork->state = UNAVAILABLE;

    sem_post(&p->app->state);
}

void freeForks(struct Philosopher *p) {
    struct App *app = p->app;

    sem_wait(&app->state);

    p->leftFork->state = AVAILABLE;
    p->rightFork->state = AVAILABLE;

    p->completedCycles++;

    if (p->completedCycles == p->goalCycles) {
        p->isCompleted = 1;
        p->state = FINISHED;
    } else {
        p->state = THINKING;
    }

    printCycles(app);

    sem_post(&app->state);

    sem_post(&p->leftFork->semaphore);
    sem_post(&p->rightFork->semaphore);
}

void printCycles(struct App *app) {
    printf("\n=================================== Current State =================================================\n");

    printf("                                  ");
    printPhilosopher(app->philosophers[0]);
    printf("\n\n                 ");
    printForks(app->philosophers[0]->leftFork);
    printf("                            ");
    printForks(app->philosophers[0]->rightFork);

    printf("\n\n");
    printPhilosopher(app->philosophers[4]);
    printf("                                         ");
    printPhilosopher(app->philosophers[1]);

    printf("\n\n                 ");
    printForks(app->philosophers[4]->leftFork);
    printf("                            ");
    printForks(app->philosophers[1]->rightFork);

    printf("\n\n            ");
    printPhilosopher(app->philosophers[3]);
    printf("          ");
    printPhilosopher(app->philosophers[2]);

    printf("\n\n                                  ");
    printForks(app->philosophers[3]->leftFork);




    printf("\n\n===================================================================================================\n");
}

void printPhilosopher(struct Philosopher *p) {
    char *stateString = "";

    switch (p->state) {
        case HUNGRY:
            stateString = "HUNGRY";
            break;
        case THINKING:
            stateString = "THINKING";
            break;
        case EATING:
            stateString = "EATING";
            break;
        case FINISHED:
            stateString = "FINISHED";
            break;
    }

    printf("Philosopher #%d: %s.", p->position, stateString);
}

void printForks(struct Fork *f) {
    printf("Fork %d: %s", f->position, f->state == AVAILABLE ? "Available" : "In Use");
}