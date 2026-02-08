#!/bin/bash
# compare_models.sh - Spustí oba modely a porovná výsledky

echo "=============================================="
echo "  KOMPARATÍVNA ANALÝZA ENTROPIÍ"
echo "  Kybernaut-Light vs Kybernaut-Human"
echo "=============================================="

echo ""
echo "Krok 1: Spúšťam Kybernaut-Light (fyzikálny model)..."
echo "---------------------------------------------------"
sleep 2
./kybernaut_light 2>&1 | tee light_results.txt

echo ""
echo "Krok 2: Spúšťam Kybernaut-Human (model s učením)..."
echo "---------------------------------------------------"
sleep 2
./kybernaut_human 2>&1 | tee human_results.txt

echo ""
echo "=============================================="
echo "  SÚHRNNÉ POROVNANIE"
echo "=============================================="

# Extrahovanie hodnôt z výsledkov
light_delta=$(grep "Rozdiel S_thermal - S_info" light_results.txt | awk '{print $5}')
human_delta=$(grep "Rozdiel S_thermal - S_info" human_results.txt | awk '{print $5}')

light_ratio=$(grep "Pomer S_thermal/S_info" light_results.txt | awk '{print $3}')
human_ratio=$(grep "Pomer S_thermal/S_info" human_results.txt | awk '{print $3}')

light_coverage=$(grep "Pokrytie sveta" light_results.txt | grep -o "[0-9.]*%" | head -1)
human_coverage=$(grep "Pokrytie sveta" human_results.txt | grep -o "[0-9.]*%" | head -1)

echo ""
echo "KLÚČOVÉ METRIKY:"
echo "----------------"
printf "%-25s %-15s %-15s\n" "METRIKA" "LIGHT (bez učenia)" "HUMAN (s učením)"
printf "%-25s %-15s %-15s\n" "------------------------" "---------------" "---------------"
printf "%-25s %-15s %-15s\n" "ΔS (rozdiel entropií)" "$light_delta" "$human_delta"
printf "%-25s %-15s %-15s\n" "Pomer S_thermal/S_info" "$light_ratio" "$human_ratio"
printf "%-25s %-15s %-15s\n" "Pokrytie sveta" "$light_coverage" "$human_coverage"

echo ""
echo "INTERPRETÁCIA VÝSLEDKOV:"
echo "------------------------"

if (( $(echo "$human_delta < $light_delta" | bc -l) )); then
    echo "✓ ΔS_human < ΔS_light: Adaptívne učenie ZNIŽUJE entropickú neefektivitu"
    efficiency_gain=$(echo "scale=2; (($light_delta - $human_delta) / $light_delta) * 100" | bc)
    echo "  Účinnosť učenia: $efficiency_gain% zlepšenie"
else
    echo "✗ ΔS_human >= ΔS_light: Učenie NIE JE efektívnejšie ako fyzikálne zákony"
fi

if (( $(echo "$human_ratio < $light_ratio" | bc -l) )); then
    echo "✓ Pomer_human < Pomer_light: Lepšia rovnováha medzi entropiami"
else
    echo "✗ Pomer_human >= Pomer_light: Fyzikálny model je vyrovnanejší"
fi

echo ""
echo "TEORETICKÝ ZÁVER:"
echo "-----------------"
echo "1. Menší ΔS znamená efektívnejšiu premenu informácie na činnosť"
echo "2. Pomer blízky 1 znamená optimálnu rovnováhu medzi exploráciou a exploatáciou"
echo "3. Vyššie pokrytie znamená lepšiu exploráciu prostredia"
echo ""
echo "Úplné výsledky v súboroch:"
echo "  • light_results.txt"
echo "  • human_results.txt"
