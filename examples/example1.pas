program Example;
const
    Pi: double = 3.1415926;
var
    num1, num2: integer;
    Res, d: double;
begin
    num1 := 5;
    Write('Введите четное целое число: ');
    Read(num2);
    Write('Введите вещественное число: ');
    Read(d);
    if (num2 mod 2 = 0) then
    begin
        Res := (num1 - num2 * 5 div 2) / (d * 2);
        WriteLn('Результат = ', Res);
    end
    else
        WriteLn('Неверный ввод');
end.
