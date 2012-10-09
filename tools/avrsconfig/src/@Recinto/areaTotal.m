function a = areaTotal(r)
% AREATOTAL Calcula el �rea total de las superficies de un recinto
% Argumentos de entrada:
% r: recinto del cual se quiere calcular el �rea total
% Argumentos de salida:
% a: �rea total del recinto
%
% Autor: Fabi�n Tommasini
% Creado: 08/2007

planos = get(r, 'Planos');
n = length(planos);
a = 0;

for i = 1:n
    a = a + area3D(planos{i});
end