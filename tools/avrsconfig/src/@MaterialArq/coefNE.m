function coef = coefNE(m)
% COEFNE Funci�n que devuelve los coeficientes de absorci�n corregido por
% la f�rmula de Norris-Eyring
% Argumentos de entrada:
% m: objeto MaterialArq
% Argumentos de salida:
% coef: coeficientes corregidos por Norris-Eyring
%
% Autor: Fabi�n Tommasini
% Creado: 10/2007

coefS = get(m, 'CoefAbosorcion');
coef = 1 - exp(-coefS);