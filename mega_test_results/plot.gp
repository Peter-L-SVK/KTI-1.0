set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'
set output '$OUTPUT_DIR/results_plot.png'

set datafile separator ","

set multiplot layout 2,2 title "Štatistická analýza Kybernautika (n=10, svet 100×100)"

# Graf 1: ΔS porovnanie
set title "ΔS = S_{thermal} - S_{info}"
set ylabel "ΔS"
set xlabel "Testovací beh"
set style data linespoints
set xtics 1
set grid
set key left top
set yrange [0:1]

plot '< grep "Light" "$OUTPUT_DIR/summary.csv"' using 3:7 with linespoints title "Light ΔS" lc rgb "#FF6B6B" pt 7 ps 1, \
     '< grep "Human" "$OUTPUT_DIR/summary.csv"' using 3:7 with linespoints title "Human ΔS" lc rgb "#4ECDC4" pt 9 ps 1

# Graf 2: S_info porovnanie
set title "S_{info} (informačná entropia)"
set ylabel "S_info"
set xlabel "Testovací beh"
set yrange [0:1]

plot '< grep "Light" "$OUTPUT_DIR/summary.csv"' using 3:4 with linespoints title "Light S_info" lc rgb "#FF6B6B" pt 7 ps 1, \
     '< grep "Human" "$OUTPUT_DIR/summary.csv"' using 3:4 with linespoints title "Human S_info" lc rgb "#4ECDC4" pt 9 ps 1

# Graf 3: Pomer S_thermal/S_info
set title "Pomer S_{thermal}/S_{info}"
set ylabel "Pomer"
set xlabel "Testovací beh"
set yrange [0:15]

plot '< grep "Light" "$OUTPUT_DIR/summary.csv"' using 3:8 with linespoints title "Light Pomer" lc rgb "#FF6B6B" pt 7 ps 1, \
     '< grep "Human" "$OUTPUT_DIR/summary.csv"' using 3:8 with linespoints title "Human Pomer" lc rgb "#4ECDC4" pt 9 ps 1

# Graf 4: Priemerné hodnoty (jednoduchý boxplot)
set title "Priemerné hodnoty"
set ylabel "Hodnota"
set style fill solid 0.8
set boxwidth 0.5
set xtics ("ΔS" 0, "S_info" 1, "Pomer" 2) offset 0,0.5

# Použijeme inline data
plot '-' using 1:2:xtic(3) with boxes title "Light" lc rgb "#FF6B6B", \
     '-' using 1:2 with boxes title "Human" lc rgb "#4ECDC4"
0 0.8946 "ΔS"
1 0.1053 "S_info"
2 10.1956 "Pomer"
e
0 0.4535 ""
1 0.5464 ""
2 1.8959 ""
e

unset multiplot
