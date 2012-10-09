function t = calcTiempo(f)
% CALCTIEMPO Calcula el tiempo en segundos que est� la
% fuente pasada por par�metro
% Argumentos de entrada:
% f: fuente real
% Argumentos de salida:
% t: tiempo en segundos
%
% Autor: Fabi�n C. Tommasini
% Creado: 08/2007

vSonido = 343;  % en m/s
t = f.DistReceptor / vSonido;