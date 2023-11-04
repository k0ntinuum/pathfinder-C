#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<curses.h>
#define SIZE 100
#define white 7
#define yellow 3
#define red 1
#define rwhite 15
#define ryellow 11
#define rred 9
int quit_flag = 0;
int display_width = 2;
int f[SIZE*SIZE] = {0};
int m[SIZE*SIZE] = {0};
int w[SIZE*SIZE][SIZE*SIZE] = {0};
int g[SIZE] = {0};
int h[SIZE] = {0};
double T = 2.0;
int count = 0;
int size = 100;
int nodes_per_update = 100;
int N = 20;
double b = 6;
double p = 20;
double L = 10;
int max_cost = 3;
int best_C = 100;
int this_C = 0;
int hide_flag = 0;
int random_update = 1;
double increment = 0.5;

void set_color(int color)
{
	attron(COLOR_PAIR(color));
}

double random_double()
{
    return ((double)rand())/((double)RAND_MAX);
}
					
void update_weights(int reset_c)
{
	
	for (int i = 0; i < N*N; i++)
	{
		if (reset_c==1) 
		{
			best_C = 100;
			w[i][i] = -rand()%max_cost;
			for (int i = 0; i < N*N; i++) m[i] = 0;
		}
		if (i/N > 0 && i%N > 0 && i%N < N -1)
		{
			w[i][i - 1]     = b;
			w[i][i - N - 1] = b;
			w[i][i - N + 1] = b;
		}
		if (i/N == 0 && i%N > 0)   w[i][i-1] = b;
		if (i/N > 0 && i%N == 0)  w[i][i-N+1] = b;
		if (i/N > 0 && i%N == N-1)
		{
			w[i][i-1] = b;
			w[i][i-N-1] = b;
		}
		for (int j =0; j < i; j++)
		{
			if (j%N == i%N) w[i][j] = -p;
		}
	}
}

void random_lights()
{
	for (int i = 0; i < N*N; i++) f[i] = rand()%2;
}

void show_lights(int y, int x)
{
	for (int i = 0; i < N*N; i++)
	{
			move(y + i/N, x + (i%N)*display_width);
			if (f[i] == 1) set_color(ryellow); else set_color(white);
			printw("%d", -w[i][i]);

	}
}
void show_memory(int y, int x)
{
	for (int i = 0; i < N*N; i++)
	{
			move(y + i/N, x + (i%N)*display_width);
			if (m[i] == 1) set_color(rred); else set_color(white);
			printw("%d", -w[i][i]);

	}
}


double C()
{
	int sum = 0;
	for (int i =0; i < N*N; i++)
	{
		for (int j = 0; j <=i ; j++) sum += w[i][j]*f[i]*f[j];
	}
	return sum;
}
			 
void check_for_input()
{
	int input = ' ';
	input = getch();
	switch(input)
	{
		case KEY_RIGHT:
		case 'T':
			T += increment;
			break;
		case KEY_LEFT:
		case 't':
			if (T - increment >= 0) T -= increment;
			break;
		
		case KEY_UP:
		case 'I':
			increment += 0.1;
			break;
		case KEY_DOWN:
		case 'i':
			if (increment - 0.1 > 0) increment -= 0.1;
			break;
		case 'q': 
			quit_flag = 1 ;
			break;
		case 'x':
			random_lights();
			break;
		case 'P':
			p += increment;
			update_weights(0);
			break;
		case 'p':
			p -= increment;
			update_weights(0);
			break;
		case 'b':
			b -= increment;
			update_weights(0);
			break;
		case 'B':
			b += increment;
			update_weights(0);
			break;	
		case 'u':
			if (max_cost - 1 > 0) 
			{
				max_cost--;
				update_weights(1);
				best_C = 100;
			}
			break;
		case 'U':
			if (max_cost < 10)
			{
				max_cost++;
				update_weights(1);
				best_C = 100;
			}
			break;
		case 'm':
			for (int i = 0; i < N*N; i++) f[i] = m[i];
			break;
		case 'N':
			if (N+1 < SIZE) N++;
			update_weights(1);
			random_lights();
			best_C = 100;
			clear();
			break;
		case 'n':
			if (N-1 > 0) N--;
			update_weights(1);
			random_lights();
			best_C = 100;
			clear();
			break;
		case 'h':
			if (hide_flag == 0) hide_flag = 1; else hide_flag = 0;
		case 'r':
			if (random_update == 0) random_update = 1; else random_update = 0;
		case ERR:
			break;			
	}
}			

void show_menu(int i)
{
	
	set_color(white);
	if (1) 
	{
		move(N+4,0);
		printw("t =  %.1f     p = %.1f     b = %.1f   i =  %.1f   ",T,p,b,increment);
		move(N+ 4 + 1, 0);
		printw("keyboard controls : T/t/P/p/B/b/U/u/N/n/m/h/r/x/q");

	}
	else printw("\n");
}
void update_f(int i)
{
	double dC = 0;
	double a = 0;
	for (int j=0; j < i; j++) dC += w[i][j]*f[j];
	for (int j= i+1; j < N*N; j++) dC += w[j][i]*f[j];
	dC += w[i][i];
	dC *= 1 - 2*f[i];
	a = dC/T;	
	if (random_double() < 1/(1+exp(-a))) f[i] = 1 - f[i];		
}

void update_lights(int random_update)
{
	if (random_update)
	{
		for (int i = 0; i < N*N; i++) update_f(rand()%(N*N));
	}
	else
	{
		for (int i = 0; i < N*N; i++) update_f(i);
	}
}

int cost()
{
	int sum = 0;
	int bits_lit = 0;
	int columns_used = 0;
	for (int j = 0; j < N; j++) h[j] = 0;
	for (int k=0;k < N*N;k++)
	{
		if (f[k] == 1)
		{
			bits_lit++;
			g[k%N] = k/N;
			sum -= w[k][k];
			h[k%N] = 1;
		}
	}
	for (int j = 0; j < N; j++) columns_used += h[j];
	if (bits_lit != N || columns_used != N) return -1;
	for (int i = 0; i < N -1; i++)
	{
		if (fabs(g[i+1]-g[i]) > 1.0) return -1;
	}
	return sum;
}


																
void set_up_graphics()
{
	initscr();
	noecho();
	start_color();
	raw();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);
	init_pair(white, COLOR_WHITE, COLOR_BLACK);
	init_pair(yellow, COLOR_YELLOW, COLOR_BLACK);
	init_pair(red, COLOR_RED, COLOR_BLACK);
	init_pair(rwhite, COLOR_BLACK, COLOR_WHITE);
	init_pair(ryellow, COLOR_BLACK, COLOR_YELLOW);
	init_pair(rred, COLOR_BLACK, COLOR_RED);	
}				

int main()
{
	set_up_graphics();
	update_weights(1);
	random_lights();
	
	while (quit_flag == 0)
	{

		show_lights(0,0);
		this_C = cost();
		if (this_C < best_C && this_C >=0)
		{
			best_C = this_C;
			for (int i= 0; i < N*N;i++) m[i] = f[i];
			move(N +2,2*N+2); set_color(red);
			printw("<%d>    ", best_C);
		}
		show_memory(0, 2*N+2);
		show_menu(hide_flag);
		move(N+2,0); set_color(yellow);
		if (this_C != -1) printw("%d", this_C);else printw("          ");
		update_lights(random_update);
		check_for_input();
	}

} 