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

### 4. **Makefile**
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
gcc -o kybernaut_light kybernaut_light.c -lm

# Spustenie jednotlivých modelov
./kybernaut_light
# ./kybernaut_human  # predpokladaný model s učením

# Spustenie komparatívnej analýzy
chmod +x compare_models.sh
./compare_models.sh
```

### Použitie Makefile (odporúčané)
```bash
# Základné príkazy
make              # skompiluje Kybernaut-Light
make help         # zobrazí kompletnú nápovedu
make compare      # spustí komparatívnu analýzu
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

## Kompletná nápoveda Makefile

### Základné príkazy
- **`make`** - Skompiluje Kybernaut-Light (východzie pravidlo)
- **`make light`** - Skompiluje iba Light verziu
- **`make human`** - Skompiluje iba Human verziu (ak existuje)
- **`make all`** - Skompiluje všetko
- **`make clean`** - Vymaže skompilované súbory a dočasné súbory
- **`make compare`** - Spustí komparatívnu analýzu oboch modelov
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
make test
make benchmark

# Vývojový cyklus
make debug        # pre ladenie
gdb ./kybernaut_light  # ladenie v gdb
make clean && make release  # finálna verzia

# Profilovanie výkonu
make profile
./kybernaut_light
gprof ./kybernaut_light gmon.out > analysis.txt
```

### Štruktúra výstupných súborov
- **`kybernaut_light`** - Spustiteľný Light model
- **`kybernaut_human`** - Spustiteľný Human model (po vytvorení)
- **`light_results.txt`** - Výsledky simulácie Light modelu
- **`human_results.txt`** - Výsledky simulácie Human modelu
- **`kybernaut_light_v3.1_log.txt`** - Podrobný log Light modelu
- **`gmon.out`** - Profilovacie dáta (pri `make profile`)

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

## Technické špecifikácie

### Podporované platformy
- Linux/Unix systémy s Bash shellom
- Kompilátor GCC s podporou matematickej knižnice
- 64-bitová architektúra pre veľké mriežky
- Make utility (typicky GNU Make)

### Pamäťové nároky
Pamäťová náročnosť rastie kvadraticky s rozmerom mriežky:
- 100×100 mriežka: ~0.8 MB
- 500×500 mriežka: ~20 MB
- 1000×1000 mriežka: ~80 MB

### Požiadavky na systém
- GCC 4.8+ alebo Clang 3.5+
- Matematická knižnica (libm)
- Nástroj `bc` pre komparatívnu analýzu (voliteľné)
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

## Licencia a autorské práva

© 2026 Peter Leukanič. Všetky práva vyhradené. Tento projekt je určený pre výskumné a vzdelávacie účely. Pre komerčné použitie je potrebný súhlas autora.

---

*Tento projekt predstavuje prienik viacerých disciplín: fyziky, informatiky, teórie systémov a umelnej inteligencie, čím poskytuje unikátny pohľad na fundamentálne limity a možnosti inteligentných systémov v interakcii s fyzikálnym svetom. Makefile pridáva profesionálnu vrstvu automatizácie, ktorá zjednodušuje vývojový proces a sprístupňuje projekt širšej komunite výskumníkov a študentov.*
