function dibujarVector(ax, origen, destino)
% DIBUJARVECTOR Dibuja un vector (funci�n privada)
% Argumentos de entrada:
% ax: ejes donde se dibujar� el vector
% origen: punto de origen del vector
% destino: punto de destino del vector
%
% Creado por: Daniel C�spedes - V�ctor Jasa (2004)
% Modificado por: Fabi�n Tommasini (10/2006)

v = zeros(2,3);
v(1,:) = origen;
v(2,:) = destino;
axes(ax);
hold on;
plot3(v(:,1), v(:,2), v(:,3), 'k');


