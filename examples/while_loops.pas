program WhileLoops;
var
    i: integer;
begin
    i := 0;

    WriteLn('Before: ', i);

    while i<10 do
    begin
        i := i + 1;
    end;

    WriteLn('After: ', i);
end.
