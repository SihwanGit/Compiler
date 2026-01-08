let 
    int i = 0;
in 
    let
       int i = 1; int j = 2; 
    in
       print i;
       if (i>0) 
          then i=i+j; 
          else i=i-j;
       print i;
    end;
    let 
        int k = 3;
    in
        i = k;
    end;
    print i;
end;
