function t = calcTiempo(fi)
% CALCTIEMPO Calcula el tiempo en segundos que est� la
% fuente-im�genes pasada por par�metro
% Argumentos de entrada:
% fi: fuente-im�gen de la cual se quiere calcular el tiempo
% Argumentos de salida:
% t: tiempo en segundos
%
% Autor: Fabi�n C. Tommasini
% Creado: 08/2007

vSonido = 343;  % en m/s
t = fi.DistReceptor / vSonido;