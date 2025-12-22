void fill_magic(int n, int magic[225]) {
    int r, c, i, nr, nc, size;

    size = n * n;

    r = 1;
    c = (n + 1) / 2;

    i = 1;
    while (i <= size) {
        magic[(r - 1) * n + c] = i;

        nr = r - 1;
        nc = c + 1;

        if (nr < 1) nr = n;
        if (nc > n) nc = 1;

        if (magic[(nr - 1) * n + nc] != 0) {
            r = r + 1;
        } else {
            r = nr;
            c = nc;
        }

        i = i + 1;
    }
}

void print_magic(int n, int magic[225]) {
    int i, j;

    i = 1;
    while (i <= n) {
        j = 1;
        while (j <= n) {
            write(magic[(i - 1) * n + j]);
            j = j + 1;
        }
        i = i + 1;
    }
}

void clear_magic(int magic[225]) {
    int i;

    i = 0;
    while (i <= 224) {
        magic[i] = 0;
        i = i + 1;
    }
}

void main() {
    int n;
    int magic[225];

    clear_magic(magic);

    read(n);
    while (n != 0) {

        if (n < 3 || n > 15 || (n % 2) == 0) {
            read(n);
        } else {
            clear_magic(magic);
            fill_magic(n, magic);
            print_magic(n, magic);
            read(n);
        }
    }
}
