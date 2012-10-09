function m = MaterialArq(nom, coefAbs)
% MATERIALARQ Constructor de la clase MaterialArq
%   m = MaterialArq(nom, coefAbs) crea un material con el nombre y los 
%   coeficientes de absorci�n (Sabine)
%
% Autor: Fabi�n Tommasini
% Creado: 03/2007
% Modificado: 04/2007

if nargin < 2
    error('N�mero de argumentos de entrada inv�lido');   
elseif length(coefAbs) ~= 6  
    % 6 coeficientes de absorci�n para 125, 250, 500, 1000, 2000, 4000 Hz
    error('N�mero de coeficientes de absorci�n inv�lido'); 
end

if nargin == 2
    m.Nombre = nom;
    m.CoefAbsorcion = coefAbs;  % coeficientes de absorci�n Sabine
    coefAbs2 = 1 - exp(-coefAbs);  % correcci�n de Norris-Eyring
    m.CoefReflexion = sqrt(1 - coefAbs2);  % coeficientes de reflectancia
    m = class(m, 'MaterialArq');
else
    error('N�mero de argumentos de entrada inv�lido'); 
    return;
end
