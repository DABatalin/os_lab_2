#include "stdio.h"
#include "stdlib.h" 	// pid_t declaration is here
#include "unistd.h" 	// fork() and getpid() declarations are here
#include "pthread.h"
#include "time.h"       // for clock_t, clock(), CLOCKS_PER_SEC
#include <string.h>

typedef struct
{
	int experiment_num;
	int throws_number;
	int tour_number;
	int player1_start_points;
	int player2_start_points;
    int player1_wins;
	int player2_wins;
    pthread_mutex_t player1_mutex;
    pthread_mutex_t player2_mutex;
} game_parameters;


void* experiment(game_parameters *game) {
	// printf("experiment_num: %d", game->experiment_num);
	for (int j = 0; j < game->experiment_num; j++) {
		int player1_experiment_points = game->player1_start_points;
		int player2_experiment_points = game->player2_start_points;
		// srand(game->throws_number);

		for (int i = game->tour_number; i < game->throws_number; i++) {
			player1_experiment_points += rand() % 6 + 1 + rand() % 6 + 1;
			player2_experiment_points += rand() % 6 + 1 + rand() % 6 + 1;
		}
		// printf("player1_experiment_points: %d\n", player1_experiment_points);
		// printf("player2_experiment_points: %d\n", player2_experiment_points);

		if (player1_experiment_points != player2_experiment_points) {
			if (player1_experiment_points > player2_experiment_points) {
				pthread_mutex_lock(&game->player1_mutex);
				game->player1_wins += 1;
				pthread_mutex_unlock(&game->player1_mutex);
			}
			else {
				pthread_mutex_lock(&game->player2_mutex);
				game->player2_wins += 1;
				pthread_mutex_unlock(&game->player2_mutex);
			}
		}
	}
}


int main(int argc, char **argv) {
	int threads_number = 1;
	if ((argc > 3) || (argc == 2)) {
		perror("Wrong number of parameters");
		return 1;
    }

	// for (int i = 0; i < argc; i++) {
	// 	printf("%d param: %s\n", i, argv[i]);
	// }

	if (argc == 3) {
		if (strcmp(argv[1], "-t") == 0) {
			threads_number = atoi(argv[2]);
		}
		else {
			perror("No such parameter exists");
			return 2;
		}
	}
	
	double time_spent = 0.0;
	int experiment_number = 1;
	game_parameters game = {0};

	pthread_mutex_init(&game.player1_mutex, NULL);
	pthread_mutex_init(&game.player2_mutex, NULL);

	printf("Enter the number of throws (K-number): ");
	scanf("%d", &game.throws_number);
	printf("Enter the tour number: ");
	scanf("%d", &game.tour_number);
	printf("Enter the amount of points of the first player: ");
	scanf("%d", &game.player1_start_points);
	printf("Enter the amount of points of the second player: ");
	scanf("%d", &game.player2_start_points);
	printf("Enter the number of experiments: ");
	scanf("%d", &experiment_number);

	if (experiment_number < threads_number) {
		perror("The number of threads is more than number of experiments");
		return 3;
	}
	game.experiment_num = experiment_number / threads_number;

	pthread_t tid[threads_number];
	clock_t begin = clock();
    for (int i=0; i<threads_number; i++)
    {
        pthread_create(&tid[i], NULL, experiment, &game);
    }

	for (int i=0; i<threads_number; i++)
    {
        pthread_join(tid[i], NULL);
    }
	clock_t end = clock();
	double player1_chance = (double)game.player1_wins / (double)(game.player1_wins + game.player2_wins);
	double player2_chance = (double)game.player2_wins / (double)(game.player1_wins + game.player2_wins);
	printf("First player chance to win: %f\n", player1_chance);
	printf("Second player chance to win: %f\n", player2_chance);

	time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
    printf("The elapsed time is %f seconds\n", time_spent);
	return 0;

}