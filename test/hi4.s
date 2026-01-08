let 
    int i = 0;
in  
    let 
       int i; int j;
    in 
       i = 10; j = 2;
       if (j>0) 
          then i=i+j; 
          else i=i-j;
       print i;
    end;
    print i;
end;
