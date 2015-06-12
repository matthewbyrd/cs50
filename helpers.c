/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>
#include <stdio.h>

#include "helpers.h"

bool search(int value, int values[], int n)
/** Binary Search: Returns true if value is in array of n values, else false. */
{
	int min = 0;
	int max = n;
	int mid;
	while (min <= max)
	{
		mid = (min + max) / 2;
		printf("Current mid value: %d\n", values[mid]);
		if (values[mid] > value)
		{
		    max = mid - 1;
		}
		else if (values[mid] < value)
		{
			min = mid + 1;
		}
		else 
		{
			return true;
		}
	}
	return false;
}

/**
 * Selection-sorts an array of n values.
 */
void sort(int values[], int n)
{
	int smallest;
    int i, j, temp;
	for (i = 0; i < n; i++)
	{
		smallest = i;
		for (j = i; j < n; j++)
		{
			if (values[j] < values[smallest])
			{
				smallest = j;
			}
		}
		temp = values[i];
		values[i] = values[smallest];
		values[smallest] = temp;
	}
}
