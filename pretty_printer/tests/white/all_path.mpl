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
            break;
        end;
        a := a + 1;
    end;
    /* multi_operator */
    f[0] := a * 2;f[1] := a div 3;g[0] := g[1] and g[2];
    /* add_operator */
    f[0] := a + 2;
    f[1] := a - 3;
    g[0] := g[1] or g[2];
    /* relatinal_operator */
    if g[0] = true then
    if g[1] <> true then
    if e[0] < 0 then
        e[0] := a + 1
    else if e[1] <= 1 then
        e[1] := a
    else if e[2] > 2 then
    e[2] := a
    else if e[3] >= 4 then
    e[3] := a
    else
     return;
    /* number, true, false, string */
    a := 1;
    g[1] := true;
    g[2] := false;
    b := 'a';
    f := 'I have a pen.';
    /* use +/- */
    a := -1;
    a := +2 -1;
    /* iroiro */
    a := -1 * 4 * (5 +1 ) - (e[1]div 1);
    /* write */
    write ('aiueo');
    write ('I have a pen', a:1, f[1], e[1]+e[2]);
end.
