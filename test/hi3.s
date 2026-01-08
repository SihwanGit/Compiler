let
   int i = 1; int sum = 0; int n; 
in
   print "1 + 2 + ... + n?";
   read n;
   while (i <= n) { 
       sum = sum + i;
       i = i + 1;
   }
   print sum; 
end;