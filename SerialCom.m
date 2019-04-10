if ~isempty(instrfind)%finds port to use
     fclose(instrfind);
      delete(instrfind);
end

clc; %clears the previous data
s = serial('COM5'); %initializing the port for input
set(s, 'Baudrate', 9600);   %Setting the baudrate
s.Terminator = 'CR';   %defining the terminator sent by Esduino

fopen(s);   %open the port
counter = 0; %counter for x axis

NN = line(nan, nan); 

while (1)    
   
    counter = counter+1;        %increasing the x-axis
    x = get(NN, 'XD');  %define x-axis
    y = get(NN, 'YD');  %define y-axis 
    x = [x counter];      % store X axis value
    output = fscanf(s);  % read serial data as char
    output = str2double(output); %convert char to int for plot
    y = [y output];  % store data in y-axis
    set(NN, 'XD', x, 'YD', y);   %create plot
    drawnow;    %show plot
    xlabel('Time');
    ylabel('Angle')

end

fclose(s);  %close the port
delete(s);  
clear s;