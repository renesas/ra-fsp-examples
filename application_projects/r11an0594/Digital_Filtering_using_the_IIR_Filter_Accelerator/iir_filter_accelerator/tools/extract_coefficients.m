%% Coefficient Extrator
%  This script extracts the coefficients from the sosMatrix in the df2tsos 
%  filter object and creates a .txt file containing the coefficients in C
%  code format to copy into the iir_filter_coeffs_t struct

% Load the filter:
% If needed, replace 'Hbp' with the name of your .mat filter object
load('Hbp.mat');

% Number of biquad stages is filter order/2. 
% If needed, replace 'Hbp' with the name of your .mat filter object
stage = filtord(Hbp.sosMatrix)/2;

% Write the coefficients in the sosMatrix to 'IIRFAcoeff.txt' file
fileID = fopen('IIRFAcoeff.txt', 'w');

for i = 1:stage
    fprintf(fileID, '{ //stage %u\r\n', i);
    fprintf(fileID, '\t.b0 = %1.25fF,\r\n', Hbp.sosMatrix(i, 1));
    fprintf(fileID, '\t.b1 = %1.25fF,\r\n', Hbp.sosMatrix(i, 2));
    fprintf(fileID, '\t.b2 = %1.25fF,\r\n', Hbp.sosMatrix(i, 3));
    fprintf(fileID, '\t.a1 = %1.25fF,\r\n', -1*Hbp.sosMatrix(i, 5));
    fprintf(fileID, '\t.a2 = %1.25fF,\r\n', -1*Hbp.sosMatrix(i, 6));
    fprintf(fileID, '},\r\n');
end

fclose(fileID);
