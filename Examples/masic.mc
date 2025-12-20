void fill_magic(int n, int magic[225]) {
    int r, c, i;
    int nr, nc;
    int size;
    int t1, t2, t3, idx;

    size = n * n;

    r = 1;
    t1 = n + 1;
    c = t1 / 2;

    i = 1;
    while (i <= size) {

        /* idx = (r-1)*n + c */
        t1 = r - 1;
        t2 = t1 * n;
        idx = t2 + c;
        magic[idx] = i;

        nr = r - 1;
        nc = c + 1;

        if (nr < 1)
            nr = n;

        if (nc > n)
            nc = 1;

        /* idx = (nr-1)*n + nc */
        t1 = nr - 1;
        t2 = t1 * n;
        idx = t2 + nc;

        if (magic[idx] != 0) {
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
    int t1, idx;

    i = 1;
    while (i <= n) {
        t1 = (i - 1) * n;

        j = 1;
        while (j <= n) {
            idx = t1 + j;
            write(magic[idx]);
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
