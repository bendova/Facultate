function tema2

f(1) = dir('documentatie_UMAPID.doc');
f(2) = dir('documentatie_UMAPID.pdf');
f(3) = dir('Prefata_Undine.txt');
f(4) = dir('show_audio.m');
f(5) = dir('Y04.M');
f(6) = dir('instr_01.wav');
f(7) = dir('speech_01.wav');
f(8) = dir('sound_01.wav');
f(9) = dir('KARMA_DATA482#1_5_V7.mat');
f(10) = dir('quartz.dll');
f(11) = dir('WinRar.exe');
f(12) = dir('WINZIP32.EXE');

for i=1:12
    % Citim octetii din fisier
    VALUES = 0:255; % setul de valori posibile
	file = fopen(f(i).name); % deschidem fisierul
	A = (fread(file, inf, '*uint8')); % citim cuvinte de cate 8 biti
    figure
	plot(histc(A, VALUES ))
	title(strcat(f(i).name,'/8'))

    % citim word-urile din fisier
    % (EDGES2 = vector 0..65535)
    VALUES2 = 0:65535; % setul de valori posibile
    file = fopen(f(i).name); % deschidem fisierul
    A = (fread(file, inf, '*uint16')); % citim cuvinte de cate 16 biti
    figure
    plot(histc(A,VALUES2))
    title(strcat(f(i).name,'/16'))
end
end
