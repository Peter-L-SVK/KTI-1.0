# ====================================================
# KYBERNAUT PROJECT - MAKEFILE
# ====================================================
# Autor: Peter Leukanič
# Rok: 2026
#
# Použitie:
#   make              - skompiluje oba modely
#   make light        - skompiluje iba Kybernaut-Light
#   make human        - skompiluje iba Kybernaut-Human
#   make all          - skompiluje všetko
#   make clean        - vymaže skompilované súbory
#   make compare      - spustí komparatívnu analýzu
#   make mega-test    - spustí pokročilý štatistický test
#   make run-light    - spustí Kybernaut-Light
#   make run-human    - spustí Kybernaut-Human
#   make test         - spustí základný test
#   make debug        - skompiluje s debug symbolmi
#   make release      - skompiluje s optimalizáciou
#   make profile      - skompiluje pre profilovanie
# ====================================================

# -------------------------
# PREMENNÉ PROJEKTU
# -------------------------
CC = gcc
BASE_CFLAGS = -Wall -Wextra
LDFLAGS_LIGHT = -lm
LDFLAGS_HUMAN = -lm -lpthread
TARGET_LIGHT = kybernaut_light
TARGET_HUMAN = kybernaut_human
COMPARE_SCRIPT = compare_models.sh
MEGA_TEST_SCRIPT = mega_test.sh
SOURCE_LIGHT = kybernaut_light.c
SOURCE_HUMAN = kybernaut_human.c
OBJECT_LIGHT = kybernaut_light.o
OBJECT_HUMAN = kybernaut_human.o
OUTPUT_LIGHT = light_results.txt
OUTPUT_HUMAN = human_results.txt
LOG_LIGHT = kybernaut_light_v3.1_log.txt
LOG_HUMAN = kybernaut_human_v3.1_log.txt

# -------------------------
# KONFIGURÁCIE KOMILÁCIE
# -------------------------
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = -O3 -DNDEBUG -march=native
PROFILE_FLAGS = -pg -O2

# -------------------------
# PRAVIDLÁ
# -------------------------

# Východzie pravidlo - kompiluje oba modely
.PHONY: all
all: light human
	@echo "=========================================="
	@echo "  KYBERNAUT PROJEKT SKOMPILOVANÝ"
	@echo "=========================================="
	@echo "Oba modely boli úspešne skompilované:"
	@echo "  $(TARGET_LIGHT) - fyzikálny model"
	@echo "  $(TARGET_HUMAN) - model s učením"
	@echo ""
	@echo "Použite:"
	@echo "  make run-light    - spustí Kybernaut-Light"
	@echo "  make run-human    - spustí Kybernaut-Human"
	@echo "  make compare      - spustí komparatívnu analýzu"
	@echo "  make mega-test    - spustí pokročilý štatistický test"
	@echo "  make clean        - vymaže skompilované súbory"
	@echo "=========================================="

# Kompilácia Kybernaut-Light
.PHONY: light
light: $(TARGET_LIGHT)

$(TARGET_LIGHT): $(SOURCE_LIGHT)
	@echo "=========================================="
	@echo "  KOMPILÁCIA KYBERNAUT-LIGHT v3.1"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(RELEASE_FLAGS) -o $@ $< $(LDFLAGS_LIGHT)
	@chmod +x $@
	@echo "Kybernaut-Light skompilovaný"
	@echo "Použitie: ./$(TARGET_LIGHT)"
	@echo ""

# Kompilácia Kybernaut-Human
.PHONY: human
human: $(TARGET_HUMAN)

$(TARGET_HUMAN): $(SOURCE_HUMAN)
	@echo "=========================================="
	@echo "  KOMPILÁCIA KYBERNAUT-HUMAN v3.1"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(RELEASE_FLAGS) -o $@ $< $(LDFLAGS_HUMAN)
	@chmod +x $@
	@echo "Kybernaut-Human skompilovaný"
	@echo "Použitie: ./$(TARGET_HUMAN)"
	@echo ""

# Kompilácia oboch modelov
.PHONY: both
both: light human
	@echo "Oba modely skompilované"

# Spustenie komparatívnej analýzy
.PHONY: compare
compare: light human
	@echo "=========================================="
	@echo "  SPUSTENIE KOMPARATÍVNEJ ANALÝZY"
	@echo "=========================================="
	@if [ ! -x "$(COMPARE_SCRIPT)" ]; then \
		chmod +x $(COMPARE_SCRIPT); \
	fi
	@echo "Spúšťam komparáciu oboch modelov..."
	@./$(COMPARE_SCRIPT)

# Spustenie pokročilého štatistického testu
.PHONY: mega-test
mega-test: light human
	@echo "=========================================="
	@echo "  SPUSTENIE POKROČILÉHO ŠTATISTICKÉHO TESTU"
	@echo "=========================================="
	@echo "Mega test: 10× opakovanie na svete 1000×1000"
	@echo "Tento test môže trvať dlhšie kvôli veľkému svetu"
	@echo ""
	@if [ ! -x "$(MEGA_TEST_SCRIPT)" ]; then \
		chmod +x $(MEGA_TEST_SCRIPT); \
		echo "Skript mega_test.sh bol spusteniteľný"; \
	fi
	@echo "Kontrola závislostí pre mega test..."
	@if ! command -v bc > /dev/null 2>&1; then \
		echo "VAROVANIE: Nástroj 'bc' nie je nainštalovaný"; \
		echo "Mega test vyžaduje 'bc' pre štatistické výpočty"; \
		echo "Nainštalujte: sudo apt-get install bc (Debian/Ubuntu)"; \
		read -p "Pokračovať aj tak? (y/n): " -n 1 -r; \
		echo; \
		if [[ ! $$REPLY =~ ^[Yy] ]]; then \
			echo "Test prerušený"; \
			exit 1; \
		fi; \
	fi
	@echo ""
	@echo "Spúšťam mega test..."
	@./$(MEGA_TEST_SCRIPT)

# Spustenie Kybernaut-Light
.PHONY: run-light
run-light: light
	@echo "=========================================="
	@echo "  SPUSTENIE KYBERNAUT-LIGHT"
	@echo "=========================================="
	./$(TARGET_LIGHT)

# Spustenie Kybernaut-Human
.PHONY: run-human
run-human: human
	@echo "=========================================="
	@echo "  SPUSTENIE KYBERNAUT-HUMAN"
	@echo "=========================================="
	./$(TARGET_HUMAN)

# Debug verzia Kybernaut-Light
.PHONY: debug-light
debug-light: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA DEBUG VERZIE LIGHT"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(DEBUG_FLAGS) -o $(TARGET_LIGHT) $(SOURCE_LIGHT) $(LDFLAGS_LIGHT)
	@chmod +x $(TARGET_LIGHT)
	@echo "Debug verzia Light skompilovaná"
	@echo "Použite: gdb ./$(TARGET_LIGHT) na ladenie"
	@echo ""

# Debug verzia Kybernaut-Human
.PHONY: debug-human
debug-human: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA DEBUG VERZIE HUMAN"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(DEBUG_FLAGS) -o $(TARGET_HUMAN) $(SOURCE_HUMAN) $(LDFLAGS_HUMAN)
	@chmod +x $(TARGET_HUMAN)
	@echo "Debug verzia Human skompilovaná"
	@echo "Použite: gdb ./$(TARGET_HUMAN) na ladenie"
	@echo ""

# Release verzia Kybernaut-Light
.PHONY: release-light
release-light: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA RELEASE VERZIE LIGHT"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(RELEASE_FLAGS) -o $(TARGET_LIGHT) $(SOURCE_LIGHT) $(LDFLAGS_LIGHT)
	@chmod +x $(TARGET_LIGHT)
	@echo "Release verzia Light skompilovaná"
	@echo "Optimalizované pre maximálny výkon"
	@echo ""

# Release verzia Kybernaut-Human
.PHONY: release-human
release-human: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA RELEASE VERZIE HUMAN"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(RELEASE_FLAGS) -o $(TARGET_HUMAN) $(SOURCE_HUMAN) $(LDFLAGS_HUMAN)
	@chmod +x $(TARGET_HUMAN)
	@echo "Release verzia Human skompilovaná"
	@echo "Optimalizované pre maximálny výkon"
	@echo ""

# Profilovacia verzia
.PHONY: profile-light
profile-light: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA PROFILOVACEJ VERZIE LIGHT"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(PROFILE_FLAGS) -o $(TARGET_LIGHT) $(SOURCE_LIGHT) $(LDFLAGS_LIGHT)
	@chmod +x $(TARGET_LIGHT)
	@echo "Profilovacia verzia Light skompilovaná"
	@echo "Použite:"
	@echo "  ./$(TARGET_LIGHT)"
	@echo "  gprof ./$(TARGET_LIGHT) gmon.out > analysis.txt"
	@echo ""

.PHONY: profile-human
profile-human: 
	@echo "=========================================="
	@echo "  KOMPILÁCIA PROFILOVACEJ VERZIE HUMAN"
	@echo "=========================================="
	$(CC) $(BASE_CFLAGS) $(PROFILE_FLAGS) -o $(TARGET_HUMAN) $(SOURCE_HUMAN) $(LDFLAGS_HUMAN)
	@chmod +x $(TARGET_HUMAN)
	@echo "Profilovacia verzia Human skompilovaná"
	@echo "Použite:"
	@echo "  ./$(TARGET_HUMAN)"
	@echo "  gprof ./$(TARGET_HUMAN) gmon.out > analysis.txt"
	@echo ""

# Testovanie
.PHONY: test
test: light
	@echo "=========================================="
	@echo "  TESTOVANIE KYBERNAUT-LIGHT"
	@echo "=========================================="
	@echo "Test 1: Spustenie so základnými parametrami"
	@echo -e "15\n" | timeout 10 ./$(TARGET_LIGHT) > /dev/null 2>&1 && echo "✓ Test 1 prešiel" || echo "✗ Test 1 zlyhal"
	@echo ""
	@echo "Test 2: Kontrola výstupných súborov"
	@sleep 1
	@if [ -f "$(LOG_LIGHT)" ]; then \
		echo "Log súbor vytvorený: $(LOG_LIGHT)"; \
		ls -la $(LOG_LIGHT); \
	else \
		echo "Log súbor nebol vytvorený"; \
	fi
	@echo ""
	@echo "Test 3: Matematická validácia"
	@if [ -f "$(LOG_LIGHT)" ] && grep -q "Všetky metriky matematicky a fyzikálne korektné" $(LOG_LIGHT) 2>/dev/null; then \
		echo "Všetky metriky validované"; \
	else \
		echo "Validácia zlyhala alebo log súbor neexistuje"; \
	fi
	@echo ""
	@echo "Test 4: Test Kybernaut-Human"
	@if [ -f "$(TARGET_HUMAN)" ]; then \
		echo "Kybernaut-Human je skompilovaný"; \
	else \
		echo "Kybernaut-Human nie je skompilovaný - spustite 'make human'"; \
	fi

# Vyčistenie projektu
.PHONY: clean
clean:
	@echo "Čistenie projektu..."
	@rm -f $(TARGET_LIGHT) $(TARGET_HUMAN)
	@rm -f $(OBJECT_LIGHT) $(OBJECT_HUMAN)
	@rm -f $(OUTPUT_LIGHT) $(OUTPUT_HUMAN)
	@rm -f $(LOG_LIGHT) $(LOG_HUMAN)
	@rm -f gmon.out analysis.txt
	@rm -f *.o *.core
	@echo "Projekt vyčistený"

# Zobrazenie nápovedy
.PHONY: help
help:
	@echo "=========================================="
	@echo "  KYBERNAUT PROJEKT - NÁPOVEDA"
	@echo "=========================================="
	@echo "Cieľ: Komparatívna analýza entropických modelov"
	@echo ""
	@echo "Dostupné príkazy:"
	@echo "  make              - skompiluje oba modely (východzie)"
	@echo "  make light        - skompiluje iba Light verziu"
	@echo "  make human        - skompiluje iba Human verziu"
	@echo "  make both         - skompiluje oba modely"
	@echo "  make clean        - vymaže skompilované súbory"
	@echo "  make compare      - spustí komparatívnu analýzu"
	@echo "  make mega-test    - spustí pokročilý štatistický test"
	@echo "  make run-light    - spustí Kybernaut-Light"
	@echo "  make run-human    - spustí Kybernaut-Human"
	@echo "  make test         - spustí základný test"
	@echo "  make debug-light  - skompiluje debug verziu Light"
	@echo "  make debug-human  - skompiluje debug verziu Human"
	@echo "  make release-light- skompiluje release verziu Light"
	@echo "  make release-human- skompiluje release verziu Human"
	@echo "  make help         - zobrazí túto nápovedu"
	@echo ""
	@echo "Štruktúra projektu:"
	@echo "  kybernaut_light.c    - Fyzikálny model"
	@echo "  kybernaut_human.c    - Model s učením"
	@echo "  compare_models.sh    - Komparatívny skript"
	@echo "  mega_test.sh         - Pokročilý štatistický test"
	@echo "  Makefile            - Tento súbor"
	@echo ""
	@echo "Výstupné súbory:"
	@echo "  kybernaut_light     - Spustiteľný Light model"
	@echo "  kybernaut_human     - Spustiteľný Human model"
	@echo "  *_results.txt       - Výsledky simulácií"
	@echo "  *_log.txt           - Podrobné logy"
	@echo "=========================================="

# Informácie o projekte
.PHONY: info
info:
	@echo "=========================================="
	@echo "  INFORMÁCIE O PROJEKTE"
	@echo "=========================================="
	@echo "Názov: Kybernaut - Komparatívna štúdia entropických modelov"
	@echo "Autor: Peter Leukanič"
	@echo "Rok: 2026"
	@echo "Verzie modelov: 3.1"
	@echo "Architektúra: 64-bit"
	@echo ""
	@echo "Stav projektu:"
	@if [ -f "$(SOURCE_LIGHT)" ]; then \
		echo "Kybernaut-Light: Dostupné"; \
		LIGHT_LINES=$$(wc -l < "$(SOURCE_LIGHT)" 2>/dev/null || echo "0"); \
		echo "  Riadkov kódu: $$LIGHT_LINES"; \
	else \
		echo "Kybernaut-Light: Chýba"; \
	fi
	@if [ -f "$(SOURCE_HUMAN)" ]; then \
		echo "Kybernaut-Human: Dostupné"; \
		HUMAN_LINES=$$(wc -l < "$(SOURCE_HUMAN)" 2>/dev/null || echo "0"); \
		echo "Riadkov kódu: $$HUMAN_LINES"; \
	else \
		echo "Kybernaut-Human: Chýba"; \
	fi
	@echo ""
	@echo "Testovacie skripty:"
	@if [ -f "$(COMPARE_SCRIPT)" ]; then \
		echo "  compare_models.sh: Dostupné"; \
	else \
		echo "  compare_models.sh: Chýba"; \
	fi
	@if [ -f "$(MEGA_TEST_SCRIPT)" ]; then \
		echo "  mega_test.sh: Dostupné"; \
	else \
		echo "  mega_test.sh: Chýba"; \
	fi
	@echo ""
	@echo "Závislosti:"
	@echo "  Kompilátor: $(CC)"
	@echo "  Knižnice: matematická (-lm), pthread (-lpthread)"
	@echo "  Shell: Bash"
	@echo "  Mega test: bc (pre štatistické výpočty)"
	@echo "=========================================="

# Kontrola závislostí
.PHONY: check-deps
check-deps:
	@echo "=========================================="
	@echo "  KONTROLA ZÁVISLOSTÍ"
	@echo "=========================================="
	@echo "Kontrolujem prítomnosť potrebných nástrojov..."
	@echo ""
	@which $(CC) > /dev/null 2>&1 && echo "✓ Kompilátor: $(CC)" || echo "✗ Kompilátor: $(CC) nenájdený"
	@which make > /dev/null 2>&1 && echo "✓ Make: dostupný" || echo "✗ Make: nedostupný"
	@which bc > /dev/null 2>&1 && echo "✓ bc (kalkulačka): dostupná" || echo "✗ bc: nedostupná (potrebná pre komparáciu a mega test)"
	@echo ""
	@echo "Kontrola knižníc:"
	@echo "#include <pthread.h>\nint main() { return 0; }" | $(CC) -x c - -lpthread -o /dev/null 2>&1 && echo "✓ pthread knižnica: dostupná" || echo "✗ pthread knižnica: nedostupná"
	@echo ""
	@echo "Kontrola verzií:"
	@$(CC) --version 2>/dev/null | head -1 || echo "Nepodarilo sa získať verziu kompilátora"
	@echo "=========================================="

# Vytvorenie archívu projektu
.PHONY: dist
dist: clean
	@echo "Vytváram archív projektu..."
	@PROJECT_NAME="kybernaut_project_$(shell date +%Y%m%d_%H%M%S)"
	@mkdir -p dist/$$PROJECT_NAME
	@cp -p *.c *.sh Makefile README* LICENSE* 2>/dev/null dist/$$PROJECT_NAME/ || true
	@tar -czf dist/$$PROJECT_NAME.tar.gz -C dist $$PROJECT_NAME
	@rm -rf dist/$$PROJECT_NAME
	@echo "Archív vytvorený: dist/$$PROJECT_NAME.tar.gz"
	@ls -lh dist/$$PROJECT_NAME.tar.gz 2>/dev/null || echo "Archív sa nepodarilo vytvoriť"

# Inštalácia
.PHONY: install
install: all
	@echo "Inštalácia Kybernaut programov..."
	@if [ "$(shell id -u)" -eq 0 ]; then \
		install -m 755 $(TARGET_LIGHT) /usr/local/bin/kybernaut-light 2>/dev/null || \
		cp $(TARGET_LIGHT) /usr/local/bin/kybernaut-light && chmod 755 /usr/local/bin/kybernaut-light; \
		install -m 755 $(TARGET_HUMAN) /usr/local/bin/kybernaut-human 2>/dev/null || \
		cp $(TARGET_HUMAN) /usr/local/bin/kybernaut-human && chmod 755 /usr/local/bin/kybernaut-human; \
		echo "Kybernaut programy nainštalované do /usr/local/bin"; \
		echo "Použite: kybernaut-light alebo kybernaut-human"; \
	else \
		echo "Inštalácia vyžaduje root práva"; \
		echo "Spustite: sudo make install"; \
	fi

# Odinštalovanie
.PHONY: uninstall
uninstall:
	@echo "Odinštalovanie..."
	@if [ "$(shell id -u)" -eq 0 ]; then \
		rm -f /usr/local/bin/kybernaut-light /usr/local/bin/kybernaut-human; \
		echo "Kybernaut programy odinštalované"; \
	else \
		echo "Odinštalovanie vyžaduje root práva"; \
		echo "Spustite: sudo make uninstall"; \
	fi

# Benchmark
.PHONY: benchmark
benchmark: all
	@echo "=========================================="
	@echo "  BENCHMARK OBOCH MODELOV"
	@echo "=========================================="
	@echo "Spúšťam testy, môže to chvíľu trvať..."
	@echo ""
	@echo "Benchmark Kybernaut-Light (mriežka 50x50):"
	@echo "50" | timeout 30 ./$(TARGET_LIGHT) > /dev/null 2>&1 && \
	echo "✓ Test prebehol úspešne" || echo "✗ Test zlyhal alebo trval príliš dlho"
	@if [ -f "$(LOG_LIGHT)" ]; then \
		echo "  Výsledky: $$(grep "Pokrytie svet" $(LOG_LIGHT) 2>/dev/null | head -1)"; \
		rm -f $(LOG_LIGHT) 2>/dev/null; \
	fi
	@echo ""
	@echo "Benchmark Kybernaut-Human (mriežka 50x50):"
	@echo "50" | timeout 30 ./$(TARGET_HUMAN) > /dev/null 2>&1 && \
	echo "Test prebehol úspešne" || echo "✗ Test zlyhal alebo trval príliš dlho"
	@if [ -f "$(LOG_HUMAN)" ]; then \
		echo "Výsledky: $$(grep "Pokrytie svet" $(LOG_HUMAN) 2>/dev/null | head -1)"; \
		rm -f $(LOG_HUMAN) 2>/dev/null; \
	fi

# Zobrazenie štatistík kódu
.PHONY: stats
stats:
	@echo "=========================================="
	@echo "  ŠTATISTIKY KÓDU"
	@echo "=========================================="
	@if [ -f "$(SOURCE_LIGHT)" ]; then \
		echo "Kybernaut-Light (kybernaut_light.c):"; \
		echo "  Riadkov: $$(wc -l < "$(SOURCE_LIGHT)" 2>/dev/null || echo "0")"; \
		echo "  Slov: $$(wc -w < "$(SOURCE_LIGHT)" 2>/dev/null || echo "0")"; \
		echo "  Znakov: $$(wc -m < "$(SOURCE_LIGHT)" 2>/dev/null || echo "0")"; \
		echo "  Funkcií: $$(grep -c "^[a-zA-Z_].*(" "$(SOURCE_LIGHT)" 2>/dev/null || echo "0")"; \
		echo "  Komentárov: $$(grep -c "//\|/\*\|^\s*\*" "$(SOURCE_LIGHT)" 2>/dev/null || echo "0")"; \
		echo ""; \
	fi
	@if [ -f "$(SOURCE_HUMAN)" ]; then \
		echo "Kybernaut-Human (kybernaut_human.c):"; \
		echo "  Riadkov: $$(wc -l < "$(SOURCE_HUMAN)" 2>/dev/null || echo "0")"; \
		echo "  Slov: $$(wc -w < "$(SOURCE_HUMAN)" 2>/dev/null || echo "0")"; \
		echo "  Znakov: $$(wc -m < "$(SOURCE_HUMAN)" 2>/dev/null || echo "0")"; \
		echo "  Funkcií: $$(grep -c "^[a-zA-Z_].*(" "$(SOURCE_HUMAN)" 2>/dev/null || echo "0")"; \
		echo "  Komentárov: $$(grep -c "//\|/\*\|^\s*\*" "$(SOURCE_HUMAN)" 2>/dev/null || echo "0")"; \
	fi

# Vytvorenie dokumentácie
.PHONY: docs
docs:
	@echo "Vytváram dokumentáciu..."
	@mkdir -p docs
	@echo "# KYBERNAUT PROJEKT - DOKUMENTÁCIA" > docs/README.md
	@echo "## Vygenerované: $$(date)" >> docs/README.md
	@echo "" >> docs/README.md
	@echo "### Štruktúra súborov" >> docs/README.md
	@echo "\`\`\`" >> docs/README.md
	@ls -la *.c *.sh Makefile 2>/dev/null >> docs/README.md || echo "Žiadne súbory" >> docs/README.md
	@echo "\`\`\`" >> docs/README.md
	@echo "Dokumentácia vytvorená v docs/README.md"

# Rýchly test kompilácie
.PHONY: quick-test
quick-test:
	@echo "Rýchly test kompilácie..."
	@echo "Test Light modelu:"
	@$(CC) $(BASE_CFLAGS) -c $(SOURCE_LIGHT) -o test_light.o 2>&1 && \
	echo "Kompilácia Light úspešná" || echo "✗ Chyba pri kompilácii Light"
	@echo "Test Human modelu:"
	@$(CC) $(BASE_CFLAGS) -c $(SOURCE_HUMAN) -o test_human.o 2>&1 && \
	echo "Kompilácia Human úspešná" || echo "✗ Chyba pri kompilácii Human"
	@rm -f test_light.o test_human.o 2>/dev/null

# Východzie pravidlo
.DEFAULT_GOAL := all
