program error_call1;
var a,b,x : integer;
procedure p(a : char);
begin
	readln(x);
	writeln('proc of p',x);
	if x=1 then a := 'a';
	if x > 2 then a := 'b' else a := 'c';
	while x > 0 do x := x - 1
end;
var b : char;
var c : integer;
begin
	a := 1;		b := 'b';
	call p(b);
	call q(a)
end.
