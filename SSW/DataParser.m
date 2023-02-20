clc; clear; close all;

% Lucca Trapani

% 2/19/23

% This code parses the data received over Serial.  The data received is in
% the form of a .mat file with each column containing ASCII characters, and
% each row is at a timestep.

% Data naming convention:
% 'NS4Data_[Day]_[Month]_[Year]_[TestDescription].mat
fileOut = 'NS4Data_02_19_23_ParseFirstTest.mat';

% Load data:
load('data.mat');
rawData = dataUseful;
timeSteps = height(rawData);

% Data types:
% Start byte: character 'A', col 1
startCol = 1;
% Time data: 32 bit uint, broken up into 4 ASCII char, col 2:5
timeCol = 2:5;
timeOut = zeros(timeSteps, 1);
% Photodiode data: 16 bit uint, broken up into 2 ASCII char, col 6:7
pdCol = 6:7;
pdOut = timeOut;
% Threshold Flag: 1 byte uint, 1 ASCII char, col 8
thresholdCol = 8;
thresholdOut = timeOut;
% Sunset Sunrise: 1 byte uint: 1 ASCII char, col 9
sunCol = 9;
sunOut = timeOut;
% Photodiode therm: 16 bit uint, 2 ASCII char, col 10:11
pdThermCol = 10:11;
pdThermOut = timeOut;
% Analog therm: 16 bit uint, 2 ASCII char, col 12:13
analogThermCol = 12:13;
analogThermOut = timeOut;
% Digital therm: 16 bit uint, 2ASCII char, col 14:15
digitalThermCol = 14:15;
digitalThermOut = timeOut;
% Analog current: 16 bit uint, 2 ASCII char, col 16:17
analogCurrentCol = 16:17;
analogCurrentOut = timeOut;
% Digital current: 16 bit uint, 2 ASCII char, col 18:19
digitalCurrentCol = 18:19;
digitalCurrentOut = timeOut;
% End byte: character 'Z', col 20
endCol = 20;

% Parse!
for i = 1:timeSteps
    timeOut(i) = NSparse(rawData(i, :), flip(timeCol));
    pdOut(i) = NSparse(rawData(i, :), pdCol);
    thresholdOut(i) = NSparse(rawData(i, :), thresholdCol);
    sunOut(i) = NSparse(rawData(i, :), sunCol);
    pdThermOut(i) = NSparse(rawData(i, :), pdThermCol);
    analogThermOut(i) = NSparse(rawData(i, :), analogThermCol);
    digitalThermOut(i) = NSparse(rawData(i, :), digitalThermCol);
    analogCurrentOut(i) = NSparse(rawData(i, :), analogCurrentCol);
    digitalCurrentOut(i) = NSparse(rawData(i, :), digitalThermCol);
end
timeOut = timeOut - timeOut(1);

% Save data
save(fileOut, 'timeOut', 'pdOut', 'thresholdOut', 'sunOut', 'pdThermOut', 'analogThermOut', ...
    'digitalThermOut', 'analogCurrentOut', 'digitalCurrentOut');

%% Function
function [parsedDataOut] = NSparse(data, columns)
    % ASCII col vector
    binVal = '';
    
    for ii = 1:length(columns)
        binVal = [binVal, dec2bin(data(columns(ii)), 8)];
    end
    parsedDataOut = bin2dec(binVal);
end
