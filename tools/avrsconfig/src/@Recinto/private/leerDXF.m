function planos = leerDXF(ruta)
% LEERDXF Lee un archivo DXF (de AutoCAD 2000)
% Entrada:
% ruta: direcci�n donde se encuentra el archivo
% Salida:
% planos: cell array con los planos le�dos
% 
% Autor: Juan Gorgas
% Modificaciones: Fabi�n Tommasini
% Creado: 2005
% Modificado: 04/2007

fid = fopen (ruta);

if fid == -1
    error('No existe el archivo');
    return;
end

% lee el archivo l�nea a l�nea hasta terminar
np = 0;  % n�mero de planos
vertices = cell(0); % zeros(4, 3);  % pone a cero los v�rtices (en principio 4)
flag = 0;  % flag que indica si es un nuevo plano

% lectura del archivo
while feof(fid) == 0 
    linea1 = fgetl(fid);
    linea2 = fgetl(fid);

    if strcmp(linea1, '  0') 
        if strcmp(linea2, '3DFACE')            
            flag = 0;
            np = np + 1;  % un plano m�s (todav�a no se ha revisado)            
        else
            flag = 1;
        end
    end
        
    if flag == 0
        switch linea1    
            case ' 10'  % X del primer v�rtice
                vertices{np}(1,1) = str2num(linea2);

            case ' 20'  % Y del primer v�rtice
                vertices{np}(1,2) = str2num(linea2);

            case ' 30'  % Z del primer v�rtice
                vertices{np}(1,3) = str2num(linea2);

            case ' 11'  % X del segundo v�rtice
                vertices{np}(2,1) = str2num(linea2);

            case ' 21'  % Y del segundo v�rtice
                vertices{np}(2,2) = str2num(linea2);

            case ' 31'  % Z del segundo v�rtice
                vertices{np}(2,3) = str2num(linea2);

            case ' 12'  % X del tercer v�rtice
                vertices{np}(3,1) = str2num(linea2);

            case ' 22'  % Y del tercer v�rtice
                vertices{np}(3,2) = str2num(linea2);

            case ' 32'  % Z del tercer v�rtice
                vertices{np}(3,3) = str2num(linea2);

            case ' 13'  % X del cuarto v�rtice
                vertices{np}(4,1) = str2num(linea2);

            case ' 23'  % Y del cuarto v�rtice
                vertices{np}(4,2) = str2num(linea2);

            case ' 33'  % Z del cuarto v�rtice
                vertices{np}(4,3) = str2num(linea2);
                
        end  % fin del switch
    end  % fin del if
end  % fin del while

planos = cell(np, 1);  % mantiene los diferentes planos en un cell array

for i = 1:np
    p = Plano(vertices{i}, num2str(i));  % crea el plano con un nombre
    planos{i} = p;  % agrega el plano anterior al cell array
end

