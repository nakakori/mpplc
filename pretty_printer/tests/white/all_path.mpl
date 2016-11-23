program allPathProgram;
var a: integer; var b, c: char; var d: boolean;
var e: array [3] of integer;
var f: array [5] of char;
var g: array [3] of boolean;
procedure func (n: integer; bool: boolean);
var i: integer;
begin
    read (i);
    if bool = false then
        n := i
    else if bool = true then
        n := i + 1
    else
        n := 1;
    writeln;
end;
begin
    readln (f, c);
    b := f[0];
    while a > 0 do
    begin
        while b <> c do
        begin
            d := g[a];
            call func(e[a], d);
        end;
        a := a + 1;
    end;
end.
