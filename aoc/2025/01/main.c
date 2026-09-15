#include <stdio.h>

static char *error = 0;

#define S0(x) #x
#define S1(x) S0(x)

#define check(C) if(!(C)) error = __FILE__ ":" S1(__LINE__) ": check(" #C ")"

const int read_move(FILE *f) {
    char dir = getc(f);
    check(dir == 'L' || dir == 'R');
    if(error) return 0;

    int amount = 0;
    for(;;) {
        char c = getc(f);
        if(c == '\n') break;
        check(c >= '0'  && c <= '9');
        if(error) break;
        amount = amount * 10 + c - '0';
    }
    return dir == 'L' ? -amount : amount;
}

int main(void) {
    FILE *f = fopen("2501.txt", "r");
    int position = 50;
    int count = 0;
    for(;;) {
        int move = read_move(f);
        if(error) break;

        position += move;
        while(position <    0) position += 100;
        while(position >= 100) position -= 100;
        printf("Move: %d %d\n", move, position);
        if(position == 0) count++;
    }
    printf("Count: %d\n", count);
    if(error) printf("%s\n", error);
    return 0;
}
