# Kybernaut: Komparatívna štúdia entropických modelov

## Prehľad projektu

Tento projekt predstavuje sofistikovaný výpočtový rámec pre komparatívnu analýzu dvoch rôznych prístupov k simulácii a optimalizácii pohybu v prostredí: **Kybernaut-Light** (fyzikálne založený model) a **Kybernaut-Human** (model využívajúci adaptívne učenie). Cieľom je kvantitatívne porovnať efektivitu týchto dvoch prístupov prostredníctvom entropických metrík, konkrétne informačnej a tepelnej entropie, čo umožňuje hlbšie pochopenie vzájomného vzťahu medzi informáciou a termodynamikou v procesoch rozhodovania.

## Architektúra projektu

### 1. **Kybernaut-Light (kybernaut_light.c)**
Fyzikálne korektný optický model simulujúci šírenie fotónu v heterogénnom prostredí. Model implementuje reálne optické zákony vrátane:
- **Snellovho zákona** pre refrakciu na rozhraniach materiálov
- **Fresnelových koeficientov** pre odrazy
- **Beer-Lambertovho zákona** pre absorpciu svetla
- **Cauchyho disperzie** pre závislosť indexu lomu od vlnovej dĺžky
- **Kvantovo-mechanických efektov** vrátane fázových posunov a interferencie

Model používa projekciu 3D optických javov do 2D priestoru s fyzikálne konzistentnými parametrami (mikrometerové bunky, femtosekundové časové kroky). Entropické metriky sú vypočítané z distribúcie fotónov a teplotného poľa, čo umožňuje kvantifikovať efektivitu premeny informácie na usporiadanú činnosť.

### 2. **Kybernaut-Human (kybernaut_human.c)**
Model založený na adaptívnom učení, ktorý by mal reprezentovať inteligentný systém schopný optimalizovať svoju trajektóriu na základe minulých skúseností. Tento model by mal využívať algoritmy strojového učenia na minimalizáciu entropickej neefektivity pri pohybe smerom k cieľu.

### 3. **Komparatívny skript (compare_models.sh)**
Automatizovaný nástroj na spustenie oboch modelov a porovnanie ich výkonnosti. Skript extrahuje kľúčové metriky z výstupov oboch simulácií a poskytuje interpretáciu rozdielov vo výkone.

### 4. **Pokročilý testovací skript (mega_test.sh)**
Štatistický testovací rámec pre opakované spúšťanie simulácií na veľkom svete (1000×1000 buniek). Vykonáva 10 opakovaní pre každý model, analyzuje variabilitu výsledkov a generuje komplexný HTML report s grafickou vizualizáciou.

### 5. **Makefile**
Automatizačný nástroj pre kompiláciu, spúšťanie a správu projektu. Poskytuje jednotný rozhranie pre všetky bežné úlohy spojené s vývojom a testovaním.

## Kľúčové metriky a ich interpretácia

### Entropické metriky
- **ΔS (Rozdiel entropií)**: `S_thermal - S_info` – menšia hodnota indikuje efektívnejšiu premennu informácie na činnosť
- **Pomer entropií**: `S_thermal / S_info` – hodnota blízka 1 signalizuje optimálnu rovnováhu medzi exploráciou a exploatáciou
- **Pokrytie sveta**: Percento preskúmaných buniek – indikátor efektivity explorácie prostredia

### Termodynamické metriky
- **Absorbovaná energia**: Celková energia absorbovaná prostredím počas simulácie
- **Teplotný rozsah**: Minimálna a maximálna teplota dosiahnutá v systéme
- **Efektivita fotónu**: Pomer optickej dráhy k absorbovanej energii

## Spustenie a používanie

### Kompilácia a spustenie (manuálne)
```bash
# Kompilácia Kybernaut-Light
gcc -O3 -Wall -Wextra -o kybernaut_light kybernaut_light.c -lm
gcc -O3 -Wall -Wextra -o kybernaut_human kybernaut_human.c -lm

# Spustenie jednotlivých modelov
./kybernaut_light
# ./kybernaut_human  # predpokladaný model s učením

# Spustenie komparatívnej analýzy
chmod +x compare_models.sh
./compare_models.sh

# Spustenie pokročilého štatistického testu
chmod +x mega_test.sh
./mega_test.sh
```

### Použitie Makefile (odporúčané)
```bash
# Základné príkazy
make              # skompiluje Kybernaut-Light
make help         # zobrazí kompletnú nápovedu
make compare      # spustí komparatívnu analýzu
make mega-test    # spustí pokročilý štatistický test
make run-light    # spustí Kybernaut-Light
make clean        # vyčistí projekt

# Pokročilé možnosti
make debug        # skompiluje s debug symbolmi
make release      # skompiluje s optimalizáciou
make profile      # skompiluje pre profilovanie
make test         # spustí základné testy
make benchmark    # spustí benchmark rôznych veľkostí mriežky
make dist         # vytvorí archív projektu

# Správa projektu
make info         # zobrazí informácie o projekte
make check-deps   # skontroluje závislosti
make stats        # zobrazí štatistiky kódu
```

### Vstupné parametre
Pri spustení Kybernaut-Light používateľ zadá rozmer štvorcovej mriežky (typicky 15-1000). Pre veľké rozmery (>1000) systém poskytuje upozornenie na nároky na pamäť a vyžaduje potvrdenie.

## Pokročilé testovanie a štatistická analýza

### Mega Test (mega_test.sh)
Skript `mega_test.sh` vykonáva rozsiahlu štatistickú analýzu výkonnosti oboch modelov:

#### Konfigurácia testu
- **Rozmer sveta**: 1000×1000 buniek (1 000 000 buniek)
- **Počet opakovaní**: 10 pre každý model
- **Výstupný adresár**: `mega_test_results/`

#### Funkcionalita
1. **Automatizované opakovanie**: 10× spustenie každého modelu s rovnakými podmienkami
2. **Extrakcia metrík**: Automatická extrakcia entropických metrík z logov
3. **Štatistická analýza**: Výpočet priemeru, smerodajnej odchýlky a štatistickej významnosti
4. **Percentuálne zlepšenie**: Kvantifikácia vplyvu adaptívneho učenia
5. **Grafická vizualizácia**: Generovanie grafov pomocou gnuplot (ak je nainštalovaný)
6. **HTML report**: Komplexný HTML report s výsledkami a závermi

#### Spustenie a výstupy
```bash
# Spustenie mega testu
./mega_test.sh

# Alebo pomocou Makefile
make mega-test
```

#### Výstupné súbory
- **`mega_test_results/`**: Adresár so všetkými výsledkami
  - `test_run_light_*.log`: Logy jednotlivých Light testov
  - `test_run_human_*.log`: Logy jednotlivých Human testov
  - `summary.csv`: Súhrnné údaje vo formáte CSV
  - `report.html`: Komplexný HTML report s výsledkami
  - `results_plot.png`: Grafické zobrazenie výsledkov (ak je gnuplot)
  - `plot.gp`: Gnuplot skript pre generovanie grafov

#### Štatistické metódy
- **Priemer a smerodajná odchýlka**: Kvantifikácia variability výsledkov
- **T-test**: Testovanie štatistickej významnosti rozdielov medzi modelmi
- **Percentuálne zlepšenie**: Kvantifikácia efektu adaptívneho učenia
- **Vizuálna analýza**: Grafy trendov a variabilít

### Interpretácia výsledkov
Mega test poskytuje robustné dôkazy o:
1. **Konzistentnosti modelov**: Nízka variabilita medzi opakovaniami
2. **Štatistickej významnosti**: Rozdiel medzi modelmi je reálny, nie náhodný
3. **Efekte učenia**: Kvantifikácia toho, ako veľmi adaptívne učenie zlepšuje efektivitu
4. **Škálovateľnosti**: Výkon na veľkých svetoch (1000×1000)

## Kompletná nápoveda Makefile

### Základné príkazy
- **`make`** - Skompiluje Kybernaut-Light (východzie pravidlo)
- **`make light`** - Skompiluje iba Light verziu
- **`make human`** - Skompiluje iba Human verziu (ak existuje)
- **`make all`** - Skompiluje všetko
- **`make clean`** - Vymaže skompilované súbory a dočasné súbory
- **`make compare`** - Spustí komparatívnu analýzu oboch modelov
- **`make mega-test`** - Spustí pokročilý štatistický test
- **`make run-light`** - Spustí Kybernaut-Light
- **`make run-human`** - Spustí Kybernaut-Human
- **`make test`** - Spustí základný test
- **`make debug`** - Skompiluje s debug symbolmi pre ladenie
- **`make release`** - Skompiluje s optimalizáciou pre výkon
- **`make profile`** - Skompiluje pre profilovanie výkonu
- **`make help`** - Zobrazí túto nápovedu

### Pokročilé príkazy
- **`make info`** - Zobrazí informácie o projekte a jeho stave
- **`make check-deps`** - Skontroluje prítomnosť potrebných nástrojov
- **`make benchmark`** - Spustí benchmark rôznych veľkostí mriežky
- **`make stats`** - Zobrazí štatistiky kódu (počet riadkov, slov, funkcií)
- **`make docs`** - Vytvorí základnú dokumentáciu
- **`make dist`** - Vytvorí archív projektu pre distribúciu
- **`make install`** - Nainštaluje program do systému (vyžaduje sudo)
- **`make uninstall`** - Odinštaluje program zo systému (vyžaduje sudo)

### Príklady použitia
```bash
# Rýchly štart
make
make run-light

# Kompletné testovanie
make clean
make release
make mega-test

# Vývojový cyklus
make debug        # pre ladenie
gdb ./kybernaut_light  # ladenie v gdb
make clean && make release  # finálna verzia

# Profilovanie výkonu
make profile
./kybernaut_light
gprof ./kybernaut_light gmon.out > analysis.txt

# Štatistická analýza
make mega-test
firefox mega_test_results/report.html
```

### Štruktúra výstupných súborov
- **`kybernaut_light`** - Spustiteľný Light model
- **`kybernaut_human`** - Spustiteľný Human model (po vytvorení)
- **`light_results.txt`** - Výsledky simulácie Light modelu
- **`human_results.txt`** - Výsledky simulácie Human modelu
- **`kybernaut_light_v3.1_log.txt`** - Podrobný log Light modelu
- **`gmon.out`** - Profilovacie dáta (pri `make profile`)
- **`mega_test_results/`** - Výsledky pokročilého štatistického testu

## Výstup a analýza

### Okamžitý výstup
Běhom simulácie Kybernaut-Light poskytuje detailný priebehový výpis vrátane:
- Aktuálnej pozície a optických parametrov
- Hodnôt entropií v reálnom čase
- Interakcií s prostredím (odrazy, lomy)
- Dosaženie cieľov (Domov a Bar)

### Finálny výstup
Po dokončení simulácie systém poskytuje komplexný súhrn vrátane:
- Optických metrík (celková dráha, intenzita, interakcie)
- Entropickej analýzy (normalizované hodnoty 0-1)
- Termodynamických charakteristík
- Matematickej a fyzikálnej validácie výsledkov

### Komparatívny výstup
Skript `compare_models.sh` generuje štrukturované porovnanie oboch modelov s interpretáciou:
- Ktorý model dosahuje nižšiu entropickú neefektivitu
- Ktorý model lepšie vyvažuje exploráciu a exploatáciu
- Percentuálne zlepšenie vďaka adaptívnemu učeniu

### Štatistický výstup
Skript `mega_test.sh` generuje komplexnú štatistickú analýzu:
- Priemery a variabilita výsledkov
- Štatistická významnosť rozdielov
- Percentuálne zlepšenie v dôsledku adaptívneho učenia
- Grafické vizualizácie trendov
- Profesionálny HTML report

## Teoretický základ

### Informačná entropia (S_info)
Kvantifikuje neusporiadanosť v distribúcii fotónov (resp. rozhodnutí) v priestore. Nižšia hodnota indikuje koncentrované, cielene správanie.

### Tepelná entropia (S_thermal)
Reprezentuje termodynamickú neusporiadanosť systému, ktorá vzniká absorpciou energie a generáciou tepla. V kontexte simulácie odráža energetickú neefektivitu pohybu.

### Kvantová entropia (S_quantum)
Špecifická pre optický model, vyjadruje stratu koherencie fotónového stavu v dôsledku interakcií s prostredím.

## Význam a aplikácie

Tento projekt poskytuje rámec pre:
1. **Kvantitatívne porovnanie** fyzikálnych a adaptívnych prístupov k navigácii
2. **Validáciu teórií** o vzťahu informácie a termodynamiky
3. **Optimalizáciu algoritmov** na základe entropických kritérií
4. **Výučbu pokročilých konceptov** z fyziky, informatiky a systémovej teórie
5. **Automatizáciu vývoja** pomocou Makefile
6. **Štatistickú analýzu** výkonnosti pomocou robustných testovacích postupov

## Technické špecifikácie

### Podporované platformy
- Linux/Unix systémy s Bash shellom
- Kompilátor GCC s podporou matematickej knižnice
- 64-bitová architektúra pre veľké mriežky
- Make utility (typicky GNU Make)
- Nástroj `bc` pre výpočty s desatinnými číslami (pre mega test)
- Gnuplot (voliteľné, pre grafickú vizualizáciu)

### Pamäťové nároky
Pamäťová náročnosť rastie kvadraticky s rozmerom mriežky:
- 100×100 mriežka: ~0.8 MB
- 500×500 mriežka: ~20 MB
- 1000×1000 mriežka: ~80 MB

### Požiadavky na systém
- GCC 4.8+ alebo Clang 3.5+
- Matematická knižnica (libm)
- Nástroj `bc` pre komparatívnu analýzu a mega test
- Gnuplot (voliteľné) pre grafickú vizualizáciu
- Alespoň 100 MB voľného miesta na disku pre veľké simulácie

### Validácia a korektnosť
Model Kybernaut-Light obsahuje rozsiahlu validáciu na overenie:
- Matematickej korektnosti (rozsahy hodnôt 0-1)
- Fyzikálnej konzistencie (rýchlosť ≤ c, zákon zachovania energie)
- Numerickej stability

## Budúci vývoj

Plánované rozšírenia zahŕňajú:
- Implementáciu Kybernaut-Human modelu s reálnym adaptívnym učením
- Grafické užívateľské rozhranie pre vizualizáciu trajektórií
- Paralelné výpočty pre veľké mriežky
- Export výsledkov do štandardných formátov (CSV, JSON)
- Rozšírenie o viacrozmerné priestory a komplexnejšie prostredia
- Integrácia s CI/CD systémami pomocou Makefile
- Dockerizácia pre jednoduchšie nasadenie
- Rozšírenie štatistických testov o viac faktoriálne analýzy

## Licencia a autorské práva

© 2026 Peter Leukanič. Všetky práva vyhradené. Tento projekt je určený pre výskumné a vzdelávacie účely. Pre komerčné použitie je potrebný súhlas autora.

---

*Tento projekt predstavuje prienik viacerých disciplín: fyziky, informatiky, teórie systémov a umelnej inteligencie, čím poskytuje unikátny pohľad na fundamentálne limity a možnosti inteligentných systémov v interakcii s fyzikálnym svetom. Makefile a pokročilé testovacie skripty pridávajú profesionálnu vrstvu automatizácie, ktorá zjednodušuje vývojový proces a sprístupňuje projekt širšej komunite výskumníkov a študentov.*
