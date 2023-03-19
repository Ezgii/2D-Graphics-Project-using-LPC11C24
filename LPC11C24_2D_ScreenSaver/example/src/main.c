

#include "lcd__api.h"
#include "board.h"
#include <math.h>
#include <stdio.h>

#include <cr_section_macros.h>

#define PI 3.141592654
// resolution = MxN = 128x160
#define M 128
#define N 160

void rotating_squares(float lambda)
{
	uint32_t colors[] = {LIGHTBLUE, GREEN, DARKBLUE, BLACK, BLUE, RED, MAGENTA, WHITE, PURPLE};
	uint32_t color;

	float x[4][10] = {0};
	float y[4][10] = {0};

	int x_min, x_max, y_min, y_max;
	int length = 50;
	int num_levels = 10;

	srand(100);

	for(int p=0; p<10; p++)
	{
		//memset(x, 0, sizeof(x));
		//memset(y, 0, sizeof(y));

		// select a random color
		color = colors[rand() % 10];

		// set boundaries
		x_min = -M/2 + length;
		x_max = M/2;
		y_min = -N/2 + length;
		y_max = N/2;

		// initial square
		x[0][0] = x_min + rand() % (x_max - x_min + 1);
		y[0][0] = y_min + rand() % (y_max - y_min + 1);
		x[1][0] = x[0][0] - length;
		y[1][0] = y[0][0];
		x[2][0] = x[1][0];
		y[2][0] = y[1][0] - length;
		x[3][0] = x[2][0] + length;
		y[3][0] = y[2][0];

		for(int j=0; j<num_levels; j++)
		{
			// create the 4 points
			for(int i=0; i<4; i++)
			{
				x[i][j+1] = x[i][j] + lambda * (x[(i+1)%4][j] - x[i][j]);
				y[i][j+1] = y[i][j] + lambda * (y[(i+1)%4][j] - y[i][j]);
			}

			// connect the 4 points together
			for(int i=0; i<4; i++)
			{
				drawLine(x[i][j] + M/2, -1*(y[i][j]) + N/2,x[(i+1)%4][j] + M/2, -1*(y[(i+1)%4][j]) + N/2, color);
			}

		}

		// randomly change side length of the parent square
		length = 100 * (1 + rand() % 10) / 10;
	}
}

struct point
{
	float x,y;
};
struct branch
{
	struct point base, root, left, mid, right;
};


void draw_single_branch(struct point *base, struct point *root, struct point *left, struct point *mid, struct point *right)
{
	float alpha;
	float lambda = 0.8;

	mid->x = root->x + lambda * (root->x - base->x);
	mid->y = root->y + lambda * (root->y - base->y);

	drawLine(mid->x + M/2, -1*(mid->y) + N/2, root->x + M/2, -1*(root->y) + N/2, DARKGREEN);

	alpha = (330 * PI)/180;

	right->x = cos(alpha)*mid->x - sin(alpha)*mid->y + cos(alpha)*(-root->x) - sin(alpha)*(-root->y) - (-root->x);
	right->y = sin(alpha)*mid->x + cos(alpha)*mid->y + sin(alpha)*(-root->x) + cos(alpha)*(-root->y) - (-root->y);

	drawLine(right->x + M/2, -1*(right->y) + N/2, root->x + M/2, -1*(root->y) + N/2, DARKGREEN);

	alpha = (30 * PI)/180;

	left->x = cos(alpha)*mid->x - sin(alpha)*mid->y + cos(alpha)*(-root->x) - sin(alpha)*(-root->y) - (-root->x);
	left->y = sin(alpha)*mid->x + cos(alpha)*mid->y + sin(alpha)*(-root->x) + cos(alpha)*(-root->y) - (-root->y);

	drawLine(left->x + M/2, -1*(left->y) + N/2, root->x + M/2, -1*(root->y) + N/2, DARKGREEN);

}

void draw_three_branches(struct branch *init_branch, struct branch *left_branch, struct branch *mid_branch, struct branch *right_branch){

	left_branch->base = init_branch->root;
	left_branch->root = init_branch->left;
	draw_single_branch(& left_branch->base, & left_branch->root,  & left_branch->left,  & left_branch->mid,  & left_branch->right);

	mid_branch->base = init_branch->root;
	mid_branch->root = init_branch->mid;
	draw_single_branch(& mid_branch->base, & mid_branch->root,  & mid_branch->left,  & mid_branch->mid,  & mid_branch->right);

	right_branch->base = init_branch->root;
	right_branch->root = init_branch->right;;
	draw_single_branch(& right_branch->base, & right_branch->root,  & right_branch->left,  & right_branch->mid,  & right_branch->right);
}

void forest()
{
	int x_min, x_max, y_min, y_max;
	int length = 20;
	int num_levels = 5;

	srand(100);

	// set boundaries
	x_min = -M/2 + 1;
	x_max = M/2 - 1;
	y_min = -N/2;
	y_max = 0;

	struct point base, root, left, mid, right;
	struct branch init_branch;

	for(int p=0; p<10; p++)
	{
		// initial trunk
		base.x = x_min + rand() % (x_max - x_min + 1);
		base.y = y_min + rand() % (y_max - y_min + 1);
		root.x = base.x;
		root.y = base.y + length;
		drawLine(base.x + M/2, -1*(base.y) + N/2, root.x + M/2, -1*(root.y) + N/2, BROWN);

		draw_single_branch(&base, &root, &left, &mid, &right);

		init_branch.base = base;
		init_branch.root = root;
		init_branch.right = right;
		init_branch.mid = mid;
		init_branch.left = left;

		struct branch **arr;
		arr = (struct branch **) malloc(num_levels * sizeof(struct branch *));
		if(arr == NULL)
		{
			printf("malloc error");
		}

		arr[0] = (struct branch *) malloc(1 * sizeof(struct branch));
		if(arr[0] == NULL)
		{
			printf("malloc error");
		}

		arr[0][0] = init_branch;

		for(int level=1; level<num_levels; level++)
		{
			arr[level] = (struct branch *) malloc((int)pow(3, level) * sizeof(struct branch));
			if(arr[level] == NULL)
			{
				printf("malloc error");
			}

			for(int i = 0, j = 0; i<pow(3, level-1) && j < pow(3, level); i++, j+=3)
			{
				draw_three_branches(&arr[level-1][i], &arr[level][j], &arr[level][j+1], &arr[level][j+2]);
			}

			free(arr[level-1]);

		}

		free(arr[num_levels-1]);
		free(arr);
	}


}


int main(void)
{
	board_init();

	ssp0_init();

	lcd_init();

	int mode_sel = 1;
	float lambda = 0.8;

	while (1)
	{


		printf("Enter 1 for rotating squares, 2 for trees: ");
		scanf("%d", &mode_sel);

		if (mode_sel == 1)
		{
			printf(" => Rotating square pattern is selected. \n");

			printf("Enter lambda value: ");
			scanf("%f", &lambda);

			// fill background color with WHITE color
			fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);

			rotating_squares(lambda);
		}
		else if (mode_sel == 2)
		{
			printf(" => Tree pattern is selected. \n ");

			// fill background color with BLUE (sky) and BLACK (soil)
			fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);
			fillrect(0, 0, ST7735_TFTWIDTH, 0.5 * ST7735_TFTHEIGHT, BLUE);

			forest();
		}
	}

	return 0;
}
