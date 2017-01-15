program change;  /* change_count.mpl */ { compute change program. What is change? }
             { no 5, 50, 500, & 5000 yen used }
  var n, count : integer;
procedure counter(yen:integer);
begin
    while n >= yen do begin
        count := count + 1;
        n := n - yen;
    end;
    if count > 0 then
        writeln(yen:5, ' yen : ', count);
    count := 0;
end;
begin
  writeln('please input change');  readln(n);    count := 0;
  call counter(10000);
  call counter(1000);
  call counter(100);
  call counter(10);
  call counter(1);
/*  if n !=0 then break;  */
end.
