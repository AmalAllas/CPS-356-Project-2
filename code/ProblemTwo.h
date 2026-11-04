#pragma once
#include <pthread.h>
#include <semaphore.h>

//
// Created by strinejc on 10/22/2026.
//

#ifndef CODE_PROBLEMTWO_H
#define CODE_PROBLEMTWO_H
enum PhilosopherState {
    THINKING,
    HUNGRY,
    EATING,
    FINISHED
};

enum ForkState {
    AVAILABLE,
    UNAVAILABLE,
};

 struct ForkLock {
    int flag;
    int guard;
    struct Queue *queue;
};

struct Philosopher {
    int position;
    enum PhilosopherState state;
    pthread_t thread;

    struct Fork *leftFork;
    struct Fork *rightFork;
    sem_t permission;

    int goalCycles;
    int completedCycles;
    int isCompleted;

    struct App *app;
};

struct Fork {
    int position;
    enum ForkState state;

    sem_t semaphore;

    struct Fork *next;
    struct Fork *prev;
};

struct App {
    int cycles;
    struct Philosopher *philosophers[5];

    struct Fork *headFork;
    struct Fork *tailFork;

    sem_t state;
};

void problemTwo();
void initializePhilosophers(struct App *app);
void initializeForks(struct App *app);
void performCycles(struct App *app);
void *philosopherCycles(void *arg);

void setUp(struct App *app);

//helpers
void addToQueue(struct Queue *queue, struct Philosopher *philosopher);
struct Philosopher *removeFromQueue(struct Queue *queue);
void scheduler(struct App *app);
void getForks(struct Philosopher *p);
void freeForks(struct Philosopher *p);

//print helpers
void printCycles(struct App *app);
void printPhilosopher(struct Philosopher *p);
void printForks(struct Fork *f);


#endif //CODE_PROBLEMTWO_H
