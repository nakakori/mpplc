program n;
var n: array [10] of integer;
begin
 iff n[1] > 0 then
    n[1] = 0
 else
    n[2] = n[1]
end.
