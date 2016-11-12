program branchtest;
var i, j, k: integer;
begin
    if i > 10 then
        if j > 19 then
        begin
            j := j +1;
            if k < 10 then
                k := k+1
            else
                k := k+j
        end
        else
            j := i
end.
