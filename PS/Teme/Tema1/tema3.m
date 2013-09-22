function tema3

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
        file = fopen(f(i).name);
        A = (fread(file, inf, '*uint8'));
        arr = histc(A, VALUES);
        s = 0;
        for j = 1:length(arr)
            if (arr(j) ~= 0)
                freq(j) = arr(j) / length(A); % calculam frecventa de aparitie
                E(j) = (-1) * log2(freq(j)); % calculam entropia
                E(j) = ceil(E(j));
                if (E(j) > 8)
                    E(j) = 8;
                end
            else
               E(j) = 0; 
            end
           
            R(j) = 8 - E(j); % calculam redundanta simbolului
            s = s + R(j); % calculam redundanta totala
            
        end
        
        % calculam redundanta raporta la nr-ul de biti din fisier
        rdncy(i) = s / (length(A) * 8); 
        
    end
    
    plot(rdncy)
    title('Redundanta');
    %plot(sort(rdncy, 2, 'descend'))
    
end