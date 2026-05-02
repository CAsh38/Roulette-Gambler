#include <stdio.h>
#include <stdlib.h>

#define N_MAX 1000 
#define N_MIN 3

#define M_MAX 500
#define M_MIN 3

#define T_MAX 1000
#define T_MIN 1

#define symbols_MAX 9
#define symbols_MIN 1

#define R_MAX __INT_MAX__
#define R_MIN 0

static int Score = 0, combinations = 0;

void spin_the_column(int** full_band, int N, int M, int funct_mode_col)
{
    int Column_nr, positions, copy;
    if (!funct_mode_col) scanf("%d%d", &Column_nr, &positions);
    else positions = 1, Column_nr = funct_mode_col - 1;
    positions %= N;

    for(int k = 0; k < positions; k++)
    {
        for(int i = 0; i < N; i++)
        {
            int copy_elem = full_band[i][Column_nr]; 
            if (i == 0) 
            {
                // print_matrix(full_band, N, M);
                copy = full_band[0][Column_nr];
                full_band[0][Column_nr] = full_band[N-1][Column_nr];
                continue;
            }
            full_band[i][Column_nr] = copy, copy = copy_elem;;
        }
    }
}

int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

void calculate_score_horizontally(int** screen, int M)
{
    for(int i = 0; i < 3; i++)
    {
        int vector[M];
        for(int j = 0; j < M; j++) vector[j] = screen[i][j];
        // shuffle(vector, M);
        // qsort(vector, M, sizeof(int), compare);
        int record = 0, loop_record = 0, record_symbol = 0;
        for(int j = 0, array_symbol = vector[0]; j < M; j++)
        {
            if (array_symbol != vector[j])
            {
                if (loop_record > record)
                {
                    record = loop_record;
                    record_symbol = array_symbol;
                }
                loop_record = 1;
                array_symbol = vector[j];
                continue;
            }
            loop_record++;
            if (j == M-1 && loop_record > record)
            {
                record = loop_record;
                record_symbol = array_symbol;
            }
        }
        if (record >= 3) 
        {
            Score += record;
            combinations++;
            if (record_symbol == 7) Score += record;
            record = 0;
        }
        // print_matrix(screen, 3, M);
    }
}

void calculate_score_diagonally(int** screen, int M)
{
    for(int i = 2; i < M; i++)
    {
        // if(screen[2][i] == screen[1][i-1] && screen[1][i-1] == screen[0][i-2]) Score+=7
        if(screen[1][i-1] == screen[2][i]
            && screen[1][i-1] == screen[2][i-2]
            && screen[1][i-1] == screen[0][i]
            && screen[1][i-1] == screen[0][i-2]) 
            {
                Score += 21, combinations++;
                if (screen[1][i-1] == 7) Score+= 21; // The bonus
            }
        else if((screen[1][i-1] == screen[2][i]
                && screen[1][i-1] == screen[0][i-2])
                || 
                (screen[1][i-1] == screen[0][i]
                && screen[1][i-1] == screen[2][i-2]))
                {
                    Score += 7, combinations++;
                    if (screen[1][i-1] == 7) Score+= 7; // The bonus
                }
    }
}

void calculate_bonus_2(int** screen, int M)
{
    int all_elem[3 * M], counter = 0, symbol_counter = 1;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < M; j++)
        {
            all_elem[i + j + counter * M - counter] = screen[i][j];
        }
        counter++;
    }
    qsort(all_elem, 3 * M, sizeof(int), compare);
    for(int i = 0, array_symbol = all_elem[0]; i < 3 * M; i++)
    {
        if (array_symbol != all_elem[i]) symbol_counter++, array_symbol = all_elem[i];
    }
    if (symbol_counter <= 2) Score += 100;
    else if(symbol_counter <=4) Score += 15;
}


int** get_full_band(int Rows, int Columns)
{
    if(Rows > N_MAX || Columns > M_MAX) return NULL;

    int** matrix = calloc(Rows, sizeof(int*));

    if(matrix == NULL) return NULL;

    // for(int i = 0; i < Columns; i++) matrix[i] = malloc(Columns * sizeof(int));
    // ^
    // |
    // Always put correct values in for because if not: segmentation fault

    for (int i = 0; i < Rows; i++) {
        // int counter = i+1;
        matrix[i] = malloc(Columns * sizeof(int));
        for(int j = 0; j < Columns; j++)
        {
            // if (i == 0) 
            // {
            //     matrix[i][j] = 7;
            //     if (j == Columns - 1) matrix[i][j] = 2;
            //     continue;
            // }
            // matrix[i][j] = counter, counter++;
            scanf("%d", &matrix[i][j]);
            // printf("%d\n", matrix[i][j]);
            // if (matrix[i][j] == 2) printf("2 ");
        }
        // for(int j = 0; j < Columns; j++) printf("%d ", matrix[i][j]);
        // printf("\n");
    }
    return matrix;
}

void print_matrix(int** matrix, int n, int m)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
    // printf("\n");
}

int** get_screen(int** full_band, int M, int sigma)
{
    int** new_screen = malloc(3 * sizeof(int*));

    for(int i = 0; i < M; i++) new_screen[i] = calloc(M, sizeof(int));
    
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < M; j++)
        {
            if(sigma == 0) new_screen[i][j] = full_band[i][j];
            else if (sigma == 1) new_screen[i][j] = 0;
            else new_screen[i][j] = __INT_MAX__;
        }
    }
    return new_screen;
}

void calc_score_on_screen(int** full_band, int M)
{
    int** cadran = get_screen(full_band, M, 0);
    calculate_score_horizontally(cadran, M);
    calculate_score_diagonally(cadran, M);
    calculate_bonus_2(cadran, M);
    free(cadran);
}

void calculate_score_for_n_repetitions(int** band, int Rows, int Columns)
{
    int n = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        spin_the_column(band, Rows, Columns, 0);
        calc_score_on_screen(band, Columns);
    }
}

void calculate_every_possible_score(int** band, int Columns)
{
    int Score_list[(3 * Columns * 10) -1], counter = 0;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < Columns; j++)
        {
            for(int k = 0; k < 10; k++)
            {
                int** cadran = get_screen(band, Columns, 0);
                cadran[i][j] = k;
                calculate_score_horizontally(cadran, Columns);
                calculate_score_diagonally(cadran, Columns);
                calculate_bonus_2(cadran, Columns);
                Score_list[counter] = Score, Score = 0, counter++;
                free(cadran);
            }
        }
    }
    qsort(Score_list, 3 * Columns * 10, sizeof(int), compare);
    printf("%d\n", Score_list[(3 * Columns * 10) - 1]);
}

void calculate_all_score_for_2_column_spins(int** band, int Rows, int Columns)
{
    int Score_list[(Rows * Rows * (Columns - 1) * Columns / 2) + Rows], counter = 0, flag = 0;
    for(int s = 0; s < Columns; s++)
    {
        if(s == Columns-1)
        {
            flag = 1;
        }
        for(int k = s + 1 - flag; k < Columns;k++)
        {
            for(int i = 0; i < Rows; i++)
            {
                for(int j = 0; j < Rows; j++)
                {
                    // print_matrix(band, Rows, Columns);
                    spin_the_column(band, Rows, Columns, k + 1);
                    calc_score_on_screen(band, Columns);
                    // print_matrix(band, Rows, Columns);
                    // printf("\n");
                    Score_list[counter] = Score, Score = 0, combinations = 0,  counter++;
                }
                // spin_the_column(band, Rows, Columns, k + 1);
                // print_matrix(band, Rows, Columns);
                spin_the_column(band, Rows, Columns, k + 1);
                spin_the_column(band, Rows, Columns, s + 1);
                // print_matrix(band, Rows, Columns);
                if(k == s)
                {
                    break;
                    break;
                }
            }
        }
    }
    qsort(Score_list, (Rows * Rows * (Columns - 1) * Columns / 2) + Rows, sizeof(int), compare);
    printf("%d\n", Score_list[((Rows * Rows * (Columns - 1) * Columns / 2) + Rows) - 1]);
}

int calculate_shortest_path_djikstra(int** band, int Columns)
{
    // implementation of djikstra's algorithm int j = 2; j < Columns; j++

    int** cadran = get_screen(band, Columns, 0);
    int** storage1 = get_screen(band, Columns, 1);

    storage1[0][0] = 0, storage1[0][1] = abs(cadran[0][0] - cadran[0][1]);
    storage1[1][0] = abs(cadran[0][0] - cadran[1][0]);

    for(int j = 2; j < Columns; j++)
    {
        for(int i = 0; i < 3; i++)
        {
            // cadran[i][j-i]
            if(i == 0) storage1[0][j] = storage1[0][j-1] + abs(cadran[0][0] - cadran[i][j-i]);
            else if(i == 2 && j-i == 0) storage1[i][j-i] = storage1[i-1][j-i] + abs(cadran[0][0] - cadran[i][j-i]);
            else if(storage1[i-1][j-i] < storage1[i][j-i-1]) 
                storage1[i][j-i] = storage1[i-1][j-i] + abs(cadran[0][0] - cadran[i][j-i]);
            else storage1[i][j-i] = storage1[i][j-i-1] + abs(cadran[0][0] - cadran[i][j-i]);
            // printf("%d\n", storage1[i][j-i]);
        }
    }

    if(storage1[0][Columns-1] < storage1[1][Columns-2])
        storage1[1][Columns-1] = storage1[0][Columns-1] + abs(cadran[0][0] - cadran[1][Columns-1]);
    else storage1[1][Columns-1] = storage1[1][Columns-2] + abs(cadran[0][0] - cadran[1][Columns-1]);

    if(storage1[1][Columns-2] < storage1[2][Columns-3])
        storage1[2][Columns-2] = storage1[1][Columns-2] + abs(cadran[0][0] - cadran[2][Columns-2]);
    else storage1[2][Columns-2] = storage1[2][Columns-3] + abs(cadran[0][0] - cadran[2][Columns-2]);

    if(storage1[1][Columns-1] < storage1[2][Columns-2])
        storage1[2][Columns-1] = storage1[1][Columns-1] + abs(cadran[0][0] - cadran[2][Columns-1]);
    else storage1[2][Columns-1] = storage1[2][Columns-2] + abs(cadran[0][0] - cadran[2][Columns-1]);

    // printf("%d", storage1[2][Columns-1]);
    int sigma = storage1[2][Columns-1];
    free(cadran), free(storage1);
    return sigma;
}

int calculate_shortest_path_DFS(int** band, int Rows, int Columns)
{
    int** cadran = get_screen(band, Columns, 0);
    int** storage1 = get_screen(band, Columns, 2);
    // int** storage2 = get_screen(band, Columns, 2);
    storage1[0][0] = 0; //storage2[0][0] = 0;

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < Columns; j++)
        {
            if((storage1[i][j+1] > storage1[i][j] + abs(cadran[0][0] - cadran[i][j+1])) && 
                j != Columns-1)
            {
                storage1[i][j+1] = storage1[i][j] + abs(cadran[0][0] - cadran[i][j+1]);
            }
            if((storage1[i+1][j] > storage1[i][j] + abs(cadran[0][0] - cadran[i+1][j])) &&
                i != Rows-1)
            {
                storage1[i+1][j] = storage1[i][j] + abs(cadran[0][0] - cadran[i+1][j]);
            }
        }
    }

    // for(int i = 0; i < 3; i++)
    // {
    //     for(int j = Columns-1; j >= 0; j--)
    //     {
    //         if((j != Columns-1 && 
    //             storage2[i][j+1] > storage2[i][j] + abs(cadran[0][0] - cadran[i][j+1])))
    //         {
    //             storage2[i][j+1] = storage2[i][j] + abs(cadran[0][0] - cadran[i][j+1]);
    //         }
    //         if((i != Rows-1 &&
    //             storage2[i+1][j] > storage2[i][j] + abs(cadran[0][0] - cadran[i+1][j])))
    //         {
    //             storage2[i+1][j] = storage2[i][j] + abs(cadran[0][0] - cadran[i+1][j]);
    //         }
    //     }
    // }

    // printf("%d", storage1[2][Columns-1]);
    int sigma = storage1[2][Columns-1];
    free(cadran), free(storage1);
    return sigma;
}

int main(void)
{
    int** full_band;
    int N = N_MIN, M = M_MIN, Task = 0;

    scanf("%d", &Task);
    scanf("%d%d", &N, &M);
    full_band = get_full_band(N, M);
    // print_matrix(full_band, N, M);
    switch (Task)
    {
        case 1:
        {
            spin_the_column(full_band, N, M, 0);
            print_matrix(full_band, N, M);
            break;
        }
        case 2:
        {
            // print_matrix(cadran, 3, M);
            calc_score_on_screen(full_band, M);
            printf("%d\n%d\n", combinations, Score);
            break;
        }
        case 3:
        {
            calculate_score_for_n_repetitions(full_band, N, M);
            printf("%d\n", Score);
            break;
        }
        case 4:
        {
            calculate_every_possible_score(full_band, M);
            break;
        }
        case 5:
        {
            calculate_all_score_for_2_column_spins(full_band, N, M);
            break;
        }
        case 6:
        {
            // calculate_shortest_path_djikstra(full_band, M);
            // calculate_shortest_path_DFS(full_band, N, M);
            int DFS = calculate_shortest_path_DFS(full_band, N, M);
            int Dijkstra = calculate_shortest_path_djikstra(full_band, M);
            if(DFS > Dijkstra)
                printf("%d\n", Dijkstra);
            else printf("%d\n", DFS);
            break;
        }
    }
    return 0;
}