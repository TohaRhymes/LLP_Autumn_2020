#include <stdio.h>

int x[] = { 3, 5, 7, 9, 11, 13 };
int y[] = { 14, 12, 10, 8, 6, 4 };


void print_array(int* array, int size) { 
    for (int i = 0; i < size; i = i + 1)
        printf("%d\t", array[i]);
    printf("\n");

}


int scalar( int a[], int b[], size_t sz ) {
    size_t i; 
    int ans = 0;
    for ( i = 0; i < sz; i++ ) 
        ans += a[i] * b[i];
    return ans;
}

int main() {
    printf("The scalar product of:\n");
    int size = sizeof(x) / sizeof(x[0]);
    print_array (x, size);
    print_array (y, size);
    printf("is: %d\n", scalar(x, y, size));
    return 0;
}
