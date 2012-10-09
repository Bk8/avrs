function pintar(ax, pl)
% PINTARPLANO Pinta un plano determinado del recinto
% Argumentos de entrada:
% ax: Ejes donde se dibur� el plano
% pl: Plano que se pintar�

axes(ax);
v = get(pl, 'Vertices');
fill3(v(:,1), v(:,2), v(:,3), 'c');