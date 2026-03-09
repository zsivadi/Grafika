#include <stdio.h>
#include <math.h>

struct Brick {
    double a;
    double b;
    double c;
};

void set_size(double a, double b, double c, struct Brick *br);
void calc_volume(struct Brick *br);
void calc_surface(struct Brick *br);
void find_square(struct Brick *br);

int main() {

    struct Brick br = {0, 0, 0};
    set_size(5, 5, 5, &br);

    printf("A tegla meretei: %.2f, %.2f, %.2f\n", br.a, br.b, br.c);

    calc_volume(&br);
    calc_surface(&br);
    find_square(&br);

}

void set_size(double a, double b, double c, struct Brick *br) {
    if (a <= 0 || b <= 0 || c <= 0) {
        printf("Hibas bemeneti paramterek!\n");
        return;
    }

    br->a = a;
    br->b = b;
    br->c = c;
}

void calc_volume(struct Brick *br) {

    double volume = br->a * br->b * br->c;

    printf("\nA teglatest terfogata: %.2f m^3\n", volume);
}

void calc_surface(struct Brick *br) {

    double surface = 2 * br->a * br->b + 2 * br->a * br->c + 2 * br->b * br->c;

    printf("\nA teglatest felszine: %.2f m^2\n", surface);
}

void find_square(struct Brick *br) {

    if (br->a == br->b || br->a == br->c || br->b == br->c) {
        printf("\nA teglatestnek VAN negyzet alaku lapja!\n");
        return;
    }

    printf("\nA teglatestnek NINCS negyzet alaku lapja!\n");
}