#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define SIZE 5000
#define MAX_NAME_SIZE 127
#define X 0
#define Y 1

/* Global variables */
char name[MAX_NAME_SIZE];
int element_count;
double buffer[SIZE][2];

/* Function prototypes */
void file_name_read(void);
int buffer_store(void);
void data_manage_select(void);
int data_append(void);
void new_file_write(void);
void analysis_select(void);
void regression_mode(double [][2], int);
void correlation_mode(double [][2], int);
void quicksort(double [][2], int, int);
int split(double [][2], int, int);

int main(void)
{
    char ch, close;

	printf("Welcome to the program. This program will allow you to\n"
		   "edit & create data sets or perform linear regression analysis\n"
		   "on a specified data set or sort a file's data.\n");

    for( ; ; ){
        printf("\nEnter the operation you want to perform:\n"
               "\t1. Data Management Mode (Press D)\n"
               "\t2. Analysis Mode (Press A)\n"
               "\t\tPress Q to quit the program.\n"
               "Operation: ");
        scanf("%c%*c", &ch);
        switch(ch){
            case 'D':
            case 'd': data_manage_select(); break;
            case 'A':
            case 'a': analysis_select(); break;
            case 'q':
            case 'Q': printf("Do you really want to quit? (Y/N): ");
					  scanf("%c%*c", &close);
                      if(close == 'Y' || close == 'y')
                          exit(EXIT_SUCCESS);
                      else if(close == 'N' || close == 'n'){
                          continue;
					  }
        }
    }
}

/***********************************************************
 * file_name_read: Reads the file name from stdin that     *
 * 				   is passed to data reading, writing &    *
 * 				   appending functions. 				   *
 ***********************************************************/
void file_name_read(void)
{
	int i;

	printf("\nEnter the name of the file.\n"
			"The name cannot exceed %d characters including its extension.\n"
			"*.txt & *.csv files are supported.\n"
			"\nFile name: ", MAX_NAME_SIZE);
	fgets(name, MAX_NAME_SIZE, stdin);

	for(i = 0; name[i] != '\0'; i++){
		if(name[i] == '\n')
			name[i] = '\0';
	}
}

/***********************************************************
 * buffer_store: Reads the values from a specified file    *
 *               & stores the values into a 2D array for   *
 *               quick access & manipulation.              *
 ***********************************************************/
int buffer_store(void)
{
    FILE *input = fopen(name, "r");
    int i, count;

    if(input == NULL){
        fclose(input);
        return -1;
    }
    i = count = 0;
	/*********************************************************
	 * These 4 calls of fgetc() remove the four characters 	 *
	 * at the beginning of each file. 						 *
	 * *******************************************************/
    fgetc(input), fgetc(input), fgetc(input), fgetc(input);
    while(feof(input) == 0){
        fscanf(input, "%lf , %lf%*c", &buffer[i][X], &buffer[i][Y]);
        i++;
        count++;
    }
    fclose(input);

    return count - 1;
}

/**********************************************************
 * data_manage_select: Prompts the user to choose a data  *
 *                     management option. The options are *
 *                     creating a new file or appending   *
 *                     data to an existing file.          *
 **********************************************************/
void data_manage_select(void)
{
    char option, close;
    int test;

    printf("\nEnter the data management operation you\n"
           "want to perform.\n");
	printf("\t1. Append data to an existing file (Press A)\n");
	printf("\t2. Create new file (Press W)\n");
	printf("\t\tPress Q to return to the main menu.\n");

    mode:
    printf("\nOperation: ");
    scanf("%c%*c", &option);

    option_read:
    switch(option){
        case 'A':
        case 'a': test = data_append();
                  if(test < 0){
                      printf("\nNo such file exists.\n");
					  printf("Do you want to continue this operation?(Y/N): ");
					  scanf("%c%*c", &close);
					  if(close == 'Y' || close == 'y'){
						  goto option_read;
					  }
					  else return;
                  }
                  break;

        case 'W':
        case 'w': new_file_write(); break;

		case 'Q': 
		case 'q': return;

        default: printf("\nNo valid option selected. Try Again.\n"); goto mode;
    }
}

/***********************************************************
 * data_append: Appends data to an already existing file   *
 * 				obtained from stdin. The function 		   *
 * 				terminates if the file does not exist. 	   *
 ***********************************************************/
int data_append(void)
{
    FILE *fp;
    double write_buffer[2];

    file_name_read();
    fp = fopen(name, "r");
	if(fp == NULL)
		return -1;
	else fclose(fp);
	fp = fopen(name, "a");
	printf("\nEnter values of independent &\n"
			"dependent variables separated by a comma.\n"
			"Enter \"0,0\" to end entering values.\n");
    while(1){
        scanf("%lf , %lf%*c", &write_buffer[X], &write_buffer[Y]);

        if(write_buffer[X] == 0 && write_buffer[Y] == 0)
            break;

        fprintf(fp, "%.3f,%.3f\n", write_buffer[X], write_buffer[Y]);
    }
    fclose(fp);
    printf("\tOperation successfully completed.\n");

    return 1;
}

/***********************************************************
 * new_file_write: Creates a new file & the user inputs    *
 *                 data into it.                           *
 ***********************************************************/
void new_file_write(void)
{
	FILE *fp;
	double write_buffer[2];

	file_name_read();

	fp = fopen(name, "w");
	printf("\nEnter values of independent &\n"
			"dependent variables seperated by a comma.\n"
			"Enter \"0,0\" to end entering values.\n");
	fprintf(fp, "X,Y\n");
	while(1){
		scanf("%lf , %lf%*c", &write_buffer[X], &write_buffer[Y]);

		if(write_buffer[X] == 0 && write_buffer[Y] == 0)
			break;
		fprintf(fp, "%.3f,%.3f\n", write_buffer[X], write_buffer[Y]);
	}
	fclose(fp);
	printf("\tOperation successfully completed.\n");
}

/************************************************************
 * analysis_select: Prompts the user about which operation  *
 *                  they want to perform on the specified   *
 *                  data set. Options are linear regression *
 *                  analysis & graph plotting.              *
 ************************************************************/
void analysis_select(void)
{
    FILE *fp;
    char option, close, sort_name[MAX_NAME_SIZE];
    int i;

	printf("\nEnter the mode of operation you want\n"
			"to perform on the file specified.\n");
	printf("\t1. Linear Regression Analysis (Press R)\n");
	printf("\t2. Data Sorting (Press S)\n");
	printf("\t\tPress Q to return to the main menu.\n");

    mode:
    printf("\nOperation: ");
    scanf("%c%*c", &option);

	option_read:
	switch(option){
        case 'R':
        case 'r': file_name_read();
				  element_count = buffer_store();
				  if(element_count < 0){
					  printf("\nNo such file exists.\n");
					  printf("Do you want to continue with the current operation?(Y/N): ");
					  scanf("%c%*c", &close);

					  if(close == 'Y' || close == 'y')
						  goto option_read;
					  else goto mode;
				  }
                  regression_mode(buffer, element_count); break;

        case 'S':
        case 's': file_name_read();
				  element_count = buffer_store();
                  if(element_count < 0){
					  printf("\nNo such file exists.\n");
					  printf("Do you want to continue with the current operation?(Y/N): ");
					  scanf("%c%*c", &close);
					  if(close == 'Y' || close == 'y')
						  goto option_read;
					  else goto mode;
				  }
                  quicksort(buffer, 0, element_count - 1);

                  snprintf(sort_name, MAX_NAME_SIZE - 1, "sorted_%s", name);
                  fp = fopen(sort_name, "w");

                  fprintf(fp, "Sorted X,Sorted Y\n");
				  order_select:
				  printf("How to sort the data?\n"
						  "\t1. Ascending Order (Press A)\n"
						  "\t2. Descending Order (Press D)\n");
				  printf("\nOperation: ");
				  scanf("%c%*c", &option);

				  if(option == 'A' || option == 'a')
                  	  for(i = 0; i < element_count; i++)
                      	  fprintf(fp, "%.4f,%.4f\n", buffer[i][X], buffer[i][Y]);
				  else if(option == 'D' || option == 'd')
					  for(i = element_count - 1; i >= 0; i--)
						  fprintf(fp, "%.4f,%.4f\n", buffer[i][X], buffer[i][Y]);
				  else{
					  printf("Wrong option selected.\n");
					  goto order_select;
				  }
                  fclose(fp);
                  printf("\tOperation successfully completed.\n"); break;

		case 'Q': 
		case 'q': return;

        default: printf("\nNo valid option selected. Try Again.\n"); goto mode;
    }
}

/*************************************************************
 * regression_mode: Calculates & print the least square      *
 *                  regression equation for the specified    *
 *                  data set.                                *
 *************************************************************/
void regression_mode(double buffer[][2], int element_count)
{
    FILE *fp;
    double x_mean, y_mean, sum_xy, beta, alpha, square_x, calc_x, calc_y;
    int i;
    char close, reg_name[MAX_NAME_SIZE];

    for(i = x_mean = y_mean = sum_xy = square_x = 0; i < element_count; i++){
        x_mean += buffer[i][X];
        y_mean += buffer[i][Y];
        square_x += pow(buffer[i][X], 2.0);
        sum_xy += (buffer[i][X] * buffer[i][Y]);
    }
    x_mean /= element_count, y_mean /= element_count;
    beta = (sum_xy - (element_count * x_mean * y_mean)) / (square_x - (element_count * pow(x_mean, 2.0)));
    alpha = y_mean - (beta * x_mean);

    printf("\nThe estimated regression equation is, y=%+.4f%+.4fx\n\n", alpha, beta);
    printf("Do you want to perform an estimation using this this equation? (Y/N): ");
    scanf("%c%*c", &close);

    if(close == 'Y' || close == 'y'){
        printf("Enter the value of the independent variable: ");
        scanf("%lf%*c", &calc_x);

        calc_y = alpha + (beta * calc_x);

        printf("The estimated value of y=%.3f\n\n", calc_y);
        printf("Do you want to save this result? (Y/N): ");
        scanf("%c%*c", &close);

        if(close == 'Y' || close == 'y'){
            snprintf(reg_name, MAX_NAME_SIZE - 1, "regression_%s", name);
            fp = fopen(reg_name, "w");

            fprintf(fp, "The estimated regression equation is y=%+.4f%+.4fx\n\n", alpha, beta);
            fprintf(fp, "For x=%.3f, y=%.3f\n", calc_x, calc_y);
            fclose(fp);
            printf("\tOperation successfully completed.\n");
        }
        else return;
    }
}
/************************************************************
 * correlation_mode: Calculates Pearson's correlation 	    *
 * 		     coefficient between the 2 variables of *
 *		     the supplied data set.		    *
 ************************************************************/
void correlation_mode(double buffer[][2], int element_count)
{
    double x_mean, y_mean, sum_xy, square_x, square_y, r;
    int i;

    for(i = x_mean = y_mean = sum_xy = square_x = 0; i < element_count; i++){
        x_mean += buffer[i][X];
        y_mean += buffer[i][Y];
        square_x += pow(buffer[i][X], 2.0);
        square_y += pow(buffer[i][Y], 2.0);
        sum_xy += (buffer[i][X] * buffer[i][Y]);
    }
    x_mean /= element_count, y_mean /= element_count;
    r = (sum_xy - (element_count * x_mean * y_mean)) / (sqrt((square_x - (element_count * pow(x_mean, 2.0))) * (square_y - (element_count * pow(y_mean, 2.0)))));

    printf("\nThe correlation co-efficient is, r = %.3f\n\n", r);
}
/*************************************************************
 * quicksort: Recursive function that sorts buffer array     *
 *            based on the values of dependent variable.     *
 *            Depends on split() to partition the array.     *
 *************************************************************/
void quicksort(double a[][2], int low, int high)
{
    int middle;

    if(low >= high)
        return;
    middle = split(a, low, high);
    quicksort(a, low, middle - 1);
    quicksort(a, middle + 1, high);
}

/*************************************************************
 * split: Complementary function to quicksort() that sorts   *
 *        the left & right sides of the array based on the   *
 *        partition element.                                 *
 *************************************************************/
int split(double a[][2], int low, int high)
{
    double part_element = a[low][Y], x_temp = a[low][X];

    for( ; ; ){
        while(low < high && part_element <= a[high][Y])
            high--;
        if(low >= high)
            break;
        a[low][X] = a[high][X];
        a[low][Y] = a[high][Y];
        low++;

        while(low < high && part_element >= a[low][Y])
            low++;
        if(low >= high)
            break;
        a[high][X] = a[low][X];
        a[high][Y] = a[low][Y];
        high--;
    }

    a[high][Y] = part_element;
    a[high][X] = x_temp;

    return high;
}
