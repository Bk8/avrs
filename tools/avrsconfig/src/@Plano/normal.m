function n = normal(p)
% NORMAL Calcula la normal unitaria al plano
% Argumentos de entrada:
% p: objeto plano
% Argumentos de salida:
% n: normal unitaria al plano (vector fila tridimensional)
%
% Autor: Fabi�n Tommasini
% Creado: 12/2006

% c�lculo de la normal
coef = coeficientes(p);
n = coef(1:3);  % (A, B, C) es la normal al plano
n = -n / norm(n);  % se normaliza para conseguir la normal unitaria (se hace negativo para los c�lculos)

%TODO: verificar si el punto pertenece a la superficie delimitada del plano
%TODO: falta el c�lculo con otro punto