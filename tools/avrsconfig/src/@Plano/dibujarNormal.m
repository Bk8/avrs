function dibujarNormal(ax, p)
% DIBUJARNORMAL Dibuja la normal al plano pasado por par�metro
% Argumentos de entrada:
% ax: ejes donde se dibujar� la normal
% p: objeto plano
%
% Fabi�n Tommasini (10/2006)

% calcula el destino del vector unitario que es la normal
% if distanciaOrigen(p) > 0
%     s = -1;
% else
%     s = 1;
% end

c = centro(p);
dest = 5 * normal(p) + c;
dibujarVector(ax, c, dest);