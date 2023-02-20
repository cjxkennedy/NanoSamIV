clc; clear; close all;

% Lucca Trapani

% 2/19/23

% Script generates a matrix of decimal (ASCII characters which I can feed
% into parser to test). Rows are time steps, columns are data
timeSteps = 100;

timeData = 0:timeSteps;

startData = 'A'.*ones(timeSteps, 1);
pdData = 100.*ones(timeSteps, 1);
threshData = zeros(timeSteps, 1);
sunData = zeros(timeSteps, 1);
pdThermData = 50.*ones(timeSteps, 1);
analogThermData = 3.*ones(timeSteps, 1);
digitalThermData = analogThermData;
analogCurrentData = analogThermData;
digitalCurrentData = analogThermData;
endData = 'Z'.*ones(timeSteps, 1);

dataDouble = [startData, pdData, threshData, sunData, pdThermData, analogThermData, digitalThermData, ...
    analogCurrentData, digitalCurrentData, endData];
dataASCII = zeros(timeSteps, 20);

for ii = 1:timeSteps
    % Start bit
    dataASCII(ii, 1) = startData(ii);

    % Time data
    bin = dec2bin(timeData(ii), 32);
    ind = 1;
    for j = 2:5
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %pd data
    bin = dec2bin(pdData(ii), 16);
    ind = 1;
    for j = 6:7
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %thresh data
    bin = dec2bin(threshData(ii), 8);
    ind = 1;
    for j = 8:8
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    % sun data
    bin = dec2bin(sunData(ii), 8);
    ind = 1;
    for j = 9:9
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %pd therm data
    bin = dec2bin(pdThermData(ii), 16);
    ind = 1;
    for j = 10:11
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %analog therm data
    bin = dec2bin(analogThermData(ii), 16);
    ind = 1;
    for j = 12:13
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %digital therm data
    bin = dec2bin(digitalThermData(ii), 16);
    ind = 1;
    for j = 14:15
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %analog current
    bin = dec2bin(analogCurrentData(ii), 16);
    ind = 1;
    for j = 16:17
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    %digital current
    bin = dec2bin(digitalCurrentData(ii), 16);
    ind = 1;
    for j = 18:19
        dataASCII(ii, j) = bin2dec(bin(ind:ind+7));
        ind = ind+8;
    end

    % end bit
    dataASCII(ii, 1) = endData(ii);

    


end

% Save as .mat file
save('TestData.mat', "dataASCII");

