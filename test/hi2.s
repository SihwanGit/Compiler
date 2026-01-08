let
   int i; int j; 
in
   i = 1; 
   print "2^n ? ";
   read j;
   while (j > 0) {
       i = i * 2; 
       j = j - 1;
   }
   print i; 
end;