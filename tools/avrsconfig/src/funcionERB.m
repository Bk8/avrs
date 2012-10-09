function wt = funcionERB(fs, n)
% FUNCIONERB Funci�n de ponderaci�n (ERB)
% Argumentos de entrada:
% fs: frecuencia de sampleo
% n: puntos de resoluci�n
% Argumentos de salida:
% wt: pesos normalizados de la funci�n ERB
%
% Autor: Fabi�n Tommasini
% Creado: 04/2007

% resoluci�n en frecuencias
deltaF = fs / n;
fr = (0:deltaF:fs/2);
% f�rmula de la ERB para la frecuencia en Hz
% (no confundir con la f�rmula en la que la frecuencia est� en KHz)
erb = 0.108 * fr + 24.7; % funci�n ERB
erbinv = (1./erb);
wt = erbinv ./ max(erbinv);  % pesos normalizados entre los valores 0 y 1