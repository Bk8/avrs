function t = calcTiempoDirecta(fi, fr)
% CALCTIEMPODIRECTA Calcula el tiempo en segundos que est� la
% fuente-im�gene pasada por par�metro de la fuente directa
% Argumentos de entrada:
% fi: fuente-im�gen de la cual se quiere calcular el tiempo relativo
% fr: fuente real
% Argumentos de salida:
% t: tiempo en segundos
%
% Autor: Fabi�n C. Tommasini
% Creado: 08/2007

vSonido = 343;  % en m/s
dist = get(fr, 'DistReceptor');
t = (fi.DistReceptor - dist) / vSonido;
