# HFT_HPC_Fraktale
Projektarbeit des Modul High Performance Computing der HFT Stuttgart

# Zum compilieren des Projekts

## Sequentiell:
- kompilieren: gcc -O3 .\fractal_seq.c -o fractal_seq.exe
- starten: .\fractal_seq.exe

## OpenMP:
- kompilieren: gcc -O3 -fopenmp .\fractal_omp.c -o fractal_omp.exe
- starten: .\fractal_omp.exe

## MPI:
- kompilieren unter Linux: mpicc fractal_mpi.c -lm -o fractal_mpi.out
- starten unter Linux: mpiexec -n <anzahl Prozesse> fractal_mpi.out
- funktioniert nicht zu kompilieren: gcc -IC:\Informatik\Programme\MS_PMI_SDK\Include\ -LC:\Informatik\Programme\MS_PMI_SDK\Lib\x64\ -lmsmpi mpi-coll-comm-soln.c
- funktioniert nicht zu starten: mpiexec -n <anzahl Prozesses> fractal_mpi.exe

## GPU:
- Projekt in Visual Studio erstellen.
- In Einstellungen C/C++ -> Allgemein: Zusätzliches IncludeVerzeichnis auf CUDA\include Ordner setzen
		Linker -> Allgemein: Zusätzliche Bibliothekverzeichnisse auf CUDA\lib\x64 setzen
		Linker -> Eingabe: Zusätzliche Abhängigkeiten OpenCL.lib hinzufügen