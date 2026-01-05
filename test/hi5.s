let
    int i; int j; int k;
in
    i=1; j=1; 
    while (i<=3) {
        j=1;
        while (j<=4) {
           k = i * j; 
           print i; print j; print k;
           j=j+1;
        }
        i = i+1;
    }
end;
