let 
    int i=0;
in  
    let
       int i=1; int j=1; bool k=true;
    in
       print i;
       if (k) 
          then i=i+j;
          else i=i-j;
       print i;
    end;
    let int k=0;
    in  
        k = i + k;
    end;
    print i;
end;
