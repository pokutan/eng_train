/* Uncomment this define while compiling under MS Visual Studio
#define _CRT_SECURE_NO_WARNINGS
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

int IsCharAlphaBet(char c){
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) ? 1 : 0;
}

int StringLen(const char str[]){
    int res = 0;
    for(res; str[res] != '\0'; ++res);
    return res;
}

void ReverseSingleString(char s[], int len){
    char temp = 0;
    len--;
    for(int i = 0; i < len; i++, len--){
        temp = s[i];
        s[i] = s[len];
        s[len] = temp;
    }
}

int ReverseStrings(char buffer[]){
    /* check input */
    int i = 0;
    for(i; buffer[i] != '\0'; i++)
        if(buffer[i] != ',' && IsCharAlphaBet(buffer[i]) != 1)
            return -1;
    int j = 0;
    i = 0;
    /* first reverse all array */
    ReverseSingleString(buffer, StringLen(buffer));
    /* now reverse back every word */
    for(;;){
        if(*(buffer + j) == ',' || *(buffer + j) == '\0'){
            ReverseSingleString(buffer + i, j - i);
            i = j + 1;
        }
        /* we reached end of array, exit loop */
        if(*(buffer + j) == '\0')
            break;
        j++;
    }
    return 1;
}

void PrintStrings(const char buffer[]){
    printf("%s\n", buffer);
}

void ReverseStringsManager(char str[]){
    printf("The word list: ");
    PrintStrings(str);
    if(ReverseStrings(str) == 1){
        printf("Reverse: ");
        PrintStrings(str);
    }else
        printf("The list is illegal\n");
}

/* task 2 - find sequence */

#define MAX_ARRAY_LEN 55

/* recursive version */

void InnerIteration(const int arr[], int out_arr[], int start, int curr){
    if(arr[start] < arr[curr])
        out_arr[curr] = max(out_arr[curr], 1 + out_arr[start]);
    if(start < curr)
        InnerIteration(arr, out_arr, ++start, curr);
}

void CalcMaximumsRec(const int arr[], int out_arr[], int start, int len){
    out_arr[start] = 1;
    InnerIteration(arr, out_arr, 0, start);
    start++;
    if(start < len)
        CalcMaximumsRec(arr, out_arr, start, len);
}
/* loop version
void CalcMaximumsLoop(const int arr[], int out_arr[], int len){
    for(int i = 0; i < len; ++i) {
        out_arr[i] = 1;
        for(int j = 0; j < i; ++j)
            if(arr[j] < arr[i])
                out_arr[i] = max(out_arr[i], 1 + out_arr[j]);
    }
}
*/

int SetupArray(int arr[], int MaxNum){
    int i = 0;
    printf("Enter 0..%d numbers. Enter -1 to finish input\n", MaxNum - 1);
    for(i; i < MaxNum; ++i){
        int n = -1;
        printf("Enter %d: ", i + 1);
        scanf("%d", &n);
        if(n < -1){
            printf("%d - Wrong input\n", n);
            --i;
            continue;
        }
        if(n == -1)
            break;
        arr[i] = n;
    }
    return i;
}

void MaxSet(int arr[], int size){
    int res = 0;
    if(size > 0){
        int results[MAX_ARRAY_LEN];
        CalcMaximumsRec(arr, results, 0, size);
        res = results[0];
        for(int i = 0; i < size; ++i)
            res = max(res, results[i]);
    }
    printf("maximum sequence length = %d\n", res);
}

/* task 4 - divide by 3 */

#define MAX_NUM_STR_LEN 500

int numCounter(char str[], int num, int counter){
    if(str[0] == '\0')
        return 0;
    if(str[0] == num){
        if(counter == 0)
            counter = 1;
        else if(counter == 1)
            counter = 2;
        else if(counter == 2)
            counter = 0;
        return numCounter(str + 1, num, counter);
    }else
        return numCounter(str + 1, num, counter);

}

int DividedBy3(char str[]){
    if(str[0] == '\0')
        return 0;
    int num1 = 0, num2 = 0;
    /* counts how many times the digit '1' appears in number*/
    num1 = numCounter(str, '1', 0);
    /* counts how many times the digit '2' appears in number*/
    num2 = numCounter(str, '2', 0);
    if((num1 == 0 && num2 == 0) || (num1 == 1 && num2 == 1) || (num1 == 2 && num2 == 2))
        return 1;
    else
        return 0;
    /*
    if the func numCounter returned 0 on both checks, the number divides.
    if the func numCounter returned 1 on both checks, the number divides.
    if the func numCounter returned 1 on one of the checks, and 0 on the other, the number DOESN'T divide.

    if the func numCounter returned 2 on one check, and 0 on the other, the number DOESN'T divide.
    if the func numCounter returned 2 on one check, and 1 one the other, the number DOESN'T divide.
    if the func numCounter returned 2 on both checks, the number divides.
    */
}

int main(){
    /* 1 */
    char strings_array[] = { "abc,ABC,EdF" };
    ReverseStringsManager(strings_array);
    /* 2 */
    int arr[MAX_ARRAY_LEN];
    int Len = SetupArray(arr, MAX_ARRAY_LEN);
    MaxSet(arr, Len);
    /* 4 */
    printf("Check whether number is dividable by 3\n");
    char num_str[MAX_NUM_STR_LEN];
    for(int i = 0; i < MAX_NUM_STR_LEN; ++i)
        num_str[i] = '\0';
    int n = -1;
    for(int i = 0; i < MAX_NUM_STR_LEN && n != 0; ++i){
        printf("Enter next number or 0 to finish input\n");
        scanf("%d", &n);
        if(n > 0 && n < 4 )
            num_str[i] = (char)n;
    }
    printf("%s %s dividable by 3\n", num_str, DividedBy3(num_str) == 1 ? "is" : "is not");
    return 0;
}
