#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstring>
using namespace std;

// Количество философов и вилок в кругу.
const int CIRCLE_SIZE = 5;

// Каждая вилка - двоичный семафор.
mutex forks[CIRCLE_SIZE];

// Мьютекс для блокирования операции декремента счетчика.
mutex spaghettiDecrement;

void philosopher(int id, int left_fork, int right_fork, int &sp_count) {
    while (true) {
        printf("[Философ %d] Хочу взять вилки.\n", id);

        forks[left_fork].lock();
        printf("[Философ %d] Я взял свою левую вилку (%d)!\n", id, left_fork);

        forks[right_fork].lock();
        printf("[Философ %d] Я взял свою правую вилку (%d)!\n", id, right_fork);

        // Одни и те же спагеттинки одновременно может взять только один поток.
        spaghettiDecrement.lock();
        if (sp_count <= 0) {
            printf("[Философ %d] Чёрт, спагетти больше не осталось. Кладу вилки обратно.\n", id);
            forks[left_fork].unlock();
            forks[right_fork].unlock();
            spaghettiDecrement.unlock();
            return;
        }
        --sp_count;
        printf("[Философ %d] Я взял вилкой спагетти. Теперь их осталось %d\n", id, sp_count);
        spaghettiDecrement.unlock();

        printf("[Философ %d] Я ем спагетти...\n", id);
        this_thread::sleep_for(chrono::milliseconds(2000)); /* Типа ест */

        printf("[Философ %d] Я поел. Освобождаю вилки %d и %d.\n", 
            id, left_fork, right_fork);
        forks[left_fork].unlock();
        forks[right_fork].unlock();

        printf("[Философ %d] *говорит что-то на филосовском...*\n", id);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "Russian");

    int spaghetti_count = 50;
    if (argc >= 3) {
        if (!strcmp(argv[1], "-n") && stoi(argv[2]) > 0) {
            spaghetti_count = stoi(argv[2]);
        }
    }

    printf("[Основной поток] Тарелка с %d условных единиц спагетти подана.\n", spaghetti_count);

    thread* philosophers[CIRCLE_SIZE];

    for (int i = 0; i < CIRCLE_SIZE; ++i) {
        // i -> left fork id;
        // (i + 1) % CIRCLE_SIZE -> right fork id.
        philosophers[i] = new thread(philosopher, 
            i, i, (i + 1) % CIRCLE_SIZE,
            std::ref(spaghetti_count));
    }

    for (int i = 0; i < CIRCLE_SIZE; ++i) {
        philosophers[i]->join();
        delete philosophers[i];
    }

    printf("[Основной поток] Трапеза окончена. Расходимся, господа.\n");

    return 0;
}