function ok = verticesCorrectos(a)
% VERTICESCORRECTOS Verifica la consistencia de los v�rtices (funci�n privada)
%
% Creado por: Fabi�n Tommasini (10/2006)

if size(a, 2) == 3
    ok = 1;
else
    ok = 0;
end