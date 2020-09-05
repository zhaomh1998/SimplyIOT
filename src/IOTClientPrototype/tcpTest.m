clear all; clc;
%% TCP
esp = tcpip('192.168.4.1', 23, 'NetworkRole', 'client');
esp.Timeout = 1;
fopen(esp);
dataRecv=[];
    %% Read and store data
    disp('Start recording data')
    fgets(esp);
    while(esp.BytesAvailable)
%         newAns = char(fread(tcpServer)')
        newAns = fgets(esp);
        fprintf(newAns)
%         newAns = str2num(newAns);
%         dataRecv = [dataRecv; newAns];
    end