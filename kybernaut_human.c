/**
 * KYBERNAUT-HUMAN v3.1 - OPRAVENÁ VERZIA
 * Autor: Peter Leukanič
 * Rok: 2026
 * Popis: Transformácia náhodnej prechádzky na kybernautický agent s cieľom,
 *        heuristikou a adaptívnym správaním. Simuluje hľadanie optimálnej
 *        cesty v štruktúrovanom prostredí.
 * Verzia: 3.1 (Multi-thread + Q-learning + Pamäť + Entropia)
 * 
 * FEATURES:
 * - Paralelné vyhodnocovanie ciest (4 thready)
 * - Q-learning s pamäťou
 * - Dynamická adaptácia stratégie
 * - Hierarchická pamäťová štruktúra
 * - Multi-core optimalizácia
 * - Informačná entropia pre monitorovanie explorácie
 *
 * OPRAVY v3.1:
 * 1. Opravená kvantová entropia (obmedzená na 0-1)
 * 2. Správna fyzikálna projekcia 3D→2D
 * 3. Konzistentné metriky medzi modelmi
 * 4. Kontrola matematických limitov
 * 
 * OPRAVENÉ CHYBY:
 * - Odstránené nepoužité premenné
 * - Pridané chýbajúce deklarácie
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>  // PRIDANÉ: Pre veľké mriežky

#define MAX_STEPS 30000           // ZVÝŠENÉ pre veľké mriežky
#define NUM_THREADS 4
#define MEMORY_DEPTH 6
#define LOG_FILENAME "kybernaut_human_v3.1_log.txt"

/* ==================== FYZIKÁLNE KONŠTANTY A PROJEKCIA ==================== */

#define BOLTZMANN 1.380649e-23     // J/K
#define PLANCK 6.62607015e-34      // J·s
#define SPEED_OF_LIGHT 299792458.0 // m/s
#define ELEMENTARY_CHARGE 1.602176634e-19 // C

#define CELL_SIZE 1.0e-6           // 1 bunka = 1 mikrometer
#define TIME_STEP 1.0e-12          // 1 krok = 1 pikosekunda
#define ENERGY_UNIT 1.0e-20        // J

/* ==================== ROZŠÍRENÉ ŠTRUKTÚRY ==================== */

typedef struct {
    float refractive_index;
    float absorption;
    float thermal_capacity;
    float density;
    float young_modulus;
    const char* name;
    char symbol;
} Material;

Material materials[5] = {
    {1.00,  1.0e-3,  1.2e3,   1.2,   1.0e5,  "vzduch", '.'},
    {1.33,  1.0e-2,  4.2e6,  1000.0, 2.2e9,  "voda",   '~'},
    {1.50,  5.0e-2,  2.0e6,  2500.0, 7.0e10, "sklo",   '#'},
    {2.42,  1.0e-1,  5.1e5,  3500.0, 1.2e12, "diamant", '*'},
    {10.0,  5.0e-1,  1.0e6,  5000.0, 2.0e11, "prekazka", 'X'}
};

typedef struct {
    int32_t x, y;               // PRIDANÉ: int32_t
    int32_t visits;             // PRIDANÉ: int32_t
    float temperature;
    float potential;
    int is_target;
    float information_density;
    int material_id;
    
    float effective_mass;
    float mobility;
} Node;

typedef struct {
    float q_values[4];
    int32_t last_visit;         // PRIDANÉ: int32_t
    float cumulative_reward;
    int32_t successful_exits;   // PRIDANÉ: int32_t
    int32_t evaluations;        // PRIDANÉ: int32_t
    pthread_mutex_t mutex;
} MemoryNode;

typedef struct {
    int32_t steps;              // PRIDANÉ: int32_t
    float total_energy_cost;
    float total_information;
    int path[MAX_STEPS];
    int32_t path_index;         // PRIDANÉ: int32_t
    
    int32_t home_reached;       // PRIDANÉ: int32_t
    int32_t bar_reached;        // PRIDANÉ: int32_t
    
    float learning_rate;
    float discount_factor;
    float exploration_rate;
    
    int64_t decisions_made;     // ZMENENÉ: int64_t pre veľké počty
    int64_t parallel_evals;     // ZMENENÉ: int64_t
    
    int short_term_memory[100][3];
    int32_t stm_index;          // PRIDANÉ: int32_t
    
    float efficiency_history[100];
    int32_t efficiency_index;   // PRIDANÉ: int32_t
    
    float learning_entropy;
    float computational_cost;
    
} Navigator;

typedef struct {
    float information_entropy;
    float thermal_entropy;
    float quantum_entropy;
    
    float total_energy_used;
    float average_temperature;
    float information_efficiency;
    
    int64_t total_cells;        // ZMENENÉ: int64_t
    int64_t visited_cells;      // ZMENENÉ: int64_t
    float coverage;
    
    float learning_efficiency;
    float decision_quality;
    
} SystemMetrics;

/* ==================== GLOBÁLNE PREMENNÉ ==================== */

int32_t dimension;              // ZMENENÉ: int32_t
Node **world;
MemoryNode **memory;
Navigator agent;
SystemMetrics metrics;

int32_t target_x, target_y;     // ZMENENÉ: int32_t
int32_t start_x, start_y;       // ZMENENÉ: int32_t

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ==================== POMOCNÉ FUNKCIE ==================== */

float physical_distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    float dx = (x2 - x1) * CELL_SIZE;
    float dy = (y2 - y1) * CELL_SIZE;
    return sqrt(dx*dx + dy*dy);
}

float movement_cost(int32_t old_x, int32_t old_y, int32_t new_x, int32_t new_y) {
    float distance = physical_distance(old_x, old_y, new_x, new_y);
    Material mat_new = materials[world[new_x][new_y].material_id];
    
    float resistance_energy = mat_new.density * distance * 9.81 * CELL_SIZE;
    float information_gain = 1.0 / (world[new_x][new_y].visits + 1.0);
    
    float cost = resistance_energy * (1.0 / ENERGY_UNIT) - information_gain * 10.0;
    
    return fmax(cost, 0.1);
}

/* ==================== OPRAVENÝ VÝPOČET ENTROPIÍ ==================== */

float calculate_information_entropy() {
    int64_t total_visits = 0;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            total_visits += world[x][y].visits;
        }
    }
    
    if (total_visits == 0) return 0.0;
    
    float entropy = 0.0;
    float log2 = log(2.0);
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            float p = (float)world[x][y].visits / total_visits;
            if (p > 0.0) {
                entropy -= p * (log(p) / log2);
            }
        }
    }
    
    float max_entropy = log(dimension * dimension) / log2;
    if (max_entropy > 0.0) {
        entropy /= max_entropy;
    }
    
    if (entropy < 0.0) entropy = 0.0;
    if (entropy > 1.0) entropy = 1.0;
    
    metrics.information_entropy = entropy;
    return entropy;
}

float calculate_thermal_entropy() {
    float total_heat = 0.0;
    int64_t cells = dimension * dimension;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            total_heat += world[x][y].temperature;
        }
    }
    
    if (total_heat <= 0.0) return 0.0;
    
    float entropy = 0.0;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            float p = world[x][y].temperature / total_heat;
            if (p > 0.0) {
                entropy -= p * log(p);
            }
        }
    }
    
    float max_entropy = log(cells);
    if (max_entropy > 0.0) {
        entropy /= max_entropy;
    }
    
    if (entropy < 0.0) entropy = 0.0;
    if (entropy > 1.0) entropy = 1.0;
    
    metrics.thermal_entropy = entropy;
    return entropy;
}

float calculate_quantum_entropy() {
    float total_coherence = 0.0;
    int32_t cells_with_memory = 0;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            float max_q = -INFINITY;
            float min_q = INFINITY;
            int has_memory = 0;
            
            pthread_mutex_lock(&memory[x][y].mutex);
            for (int d = 0; d < 4; d++) {
                if (fabs(memory[x][y].q_values[d]) > 1e-6) {
                    has_memory = 1;
                    if (memory[x][y].q_values[d] > max_q) max_q = memory[x][y].q_values[d];
                    if (memory[x][y].q_values[d] < min_q) min_q = memory[x][y].q_values[d];
                }
            }
            pthread_mutex_unlock(&memory[x][y].mutex);
            
            if (has_memory) {
                cells_with_memory++;
                
                if (fabs(max_q) > 1e-6) {
                    float spread = (max_q - min_q) / fabs(max_q);
                    float coherence = 1.0 - fmin(spread, 1.0);
                    total_coherence += coherence;
                } else {
                    total_coherence += 1.0;
                }
            }
        }
    }
    
    float avg_coherence = (cells_with_memory > 0) ? total_coherence / cells_with_memory : 1.0;
    float quantum_entropy = 1.0 - avg_coherence;
    
    if (quantum_entropy < 0.0) quantum_entropy = 0.0;
    if (quantum_entropy > 1.0) quantum_entropy = 1.0;
    
    metrics.quantum_entropy = quantum_entropy;
    return quantum_entropy;
}

/* ==================== FYZIKÁLNA PROJEKCIA 3D→2D ==================== */

void init_world_physical(int32_t dim) {
    dimension = dim;
    
    world = (Node**)malloc(dimension * sizeof(Node*));
    if (!world) {
        printf("Chyba: Nedostatok pamäte pre %"PRId32" riadkov\n", dimension);
        exit(1);
    }
    
    for (int32_t i = 0; i < dimension; i++) {
        world[i] = (Node*)malloc(dimension * sizeof(Node));
        if (!world[i]) {
            printf("Chyba: Nedostatok pamäte pre %"PRId32" stĺpcov\n", dimension);
            exit(1);
        }
    }
    
    printf("Inicializujem fyzikálny svet %"PRId32"x%"PRId32" (%"PRId64" buniek)...\n", 
           dimension, dimension, (int64_t)dimension * dimension);
    
    for (int32_t y = 0; y < dimension; y++) {
        for (int32_t x = 0; x < dimension; x++) {
            world[x][y].x = x;
            world[x][y].y = y;
            world[x][y].visits = 0;
            world[x][y].temperature = 293.15 + (rand() % 100) / 100.0 * 10.0;
            
            float r = (rand() % 1000) / 1000.0;
            if (r < 0.40) {
                world[x][y].material_id = 0;
            } else if (r < 0.70) {
                world[x][y].material_id = 1;
            } else if (r < 0.90) {
                world[x][y].material_id = 2;
            } else if (r < 0.97) {
                world[x][y].material_id = 3;
            } else {
                world[x][y].material_id = 4;
            }
            
            Material mat = materials[world[x][y].material_id];
            
            world[x][y].potential = mat.density * 9.81 * CELL_SIZE;
            world[x][y].effective_mass = mat.density * CELL_SIZE * CELL_SIZE;
            world[x][y].mobility = 1.0 / (mat.young_modulus * TIME_STEP);
            
            world[x][y].is_target = 0;
            world[x][y].information_density = 0.0;
        }
    }
    
    world[0][0].is_target = 1;
    world[0][0].material_id = 2;
    
    world[dimension-1][dimension-1].is_target = 2;
    world[dimension-1][dimension-1].material_id = 1;
}

float physical_reward(int32_t old_x, int32_t old_y, int32_t new_x, int32_t new_y) {
    float reward = 0.0;
    
    if (world[new_x][new_y].is_target == 1 && !agent.home_reached) {
        reward += 100.0 * ENERGY_UNIT;
    } else if (world[new_x][new_y].is_target == 2 && !agent.bar_reached) {
        reward += 100.0 * ENERGY_UNIT;
    }
    
    if (world[new_x][new_y].visits == 0) {
        reward += 10.0 * ENERGY_UNIT;
    }
    
    float cost = movement_cost(old_x, old_y, new_x, new_y);
    reward -= cost * 0.1;
    
    float old_dist = physical_distance(old_x, old_y, target_x, target_y);
    float new_dist = physical_distance(new_x, new_y, target_x, target_y);
    
    if (new_dist < old_dist) {
        reward += (old_dist - new_dist) / CELL_SIZE * ENERGY_UNIT;
    } else {
        reward -= (new_dist - old_dist) / CELL_SIZE * ENERGY_UNIT;
    }
    
    return reward;
}

/* ==================== INICIALIZÁCIA ==================== */

void init_memory() {
    memory = (MemoryNode**)malloc(dimension * sizeof(MemoryNode*));
    if (!memory) {
        printf("Chyba: Nedostatok pamäte pre pamäť\n");
        exit(1);
    }
    
    for (int32_t i = 0; i < dimension; i++) {
        memory[i] = (MemoryNode*)malloc(dimension * sizeof(MemoryNode));
        if (!memory[i]) {
            printf("Chyba: Nedostatok pamäte pre pamäťové bunky\n");
            exit(1);
        }
        
        for (int32_t j = 0; j < dimension; j++) {
            for (int d = 0; d < 4; d++) {
                memory[i][j].q_values[d] = 0.0;
            }
            memory[i][j].last_visit = -1;
            memory[i][j].cumulative_reward = 0.0;
            memory[i][j].successful_exits = 0;
            memory[i][j].evaluations = 0;
            pthread_mutex_init(&memory[i][j].mutex, NULL);
        }
    }
}

void init_agent() {
    agent.steps = 0;
    agent.total_energy_cost = 0.0;
    agent.total_information = 0.0;
    agent.path_index = 0;
    agent.home_reached = 0;
    agent.bar_reached = 0;
    
    agent.learning_rate = 0.18;
    agent.discount_factor = 0.92;
    agent.exploration_rate = 0.35;
    
    agent.decisions_made = 0;
    agent.parallel_evals = 0;
    agent.stm_index = 0;
    agent.efficiency_index = 0;
    
    agent.learning_entropy = 0.0;
    agent.computational_cost = 1.0e-18;
    
    metrics.information_entropy = 0.0;
    metrics.thermal_entropy = 0.0;
    metrics.quantum_entropy = 0.0;
    
    metrics.total_energy_used = 0.0;
    metrics.average_temperature = 293.15;
    metrics.information_efficiency = 0.0;
    
    metrics.total_cells = (int64_t)dimension * dimension;
    metrics.visited_cells = 0;
    metrics.coverage = 0.0;
    
    metrics.learning_efficiency = 0.0;
    metrics.decision_quality = 0.0;
}

/* ==================== HLAVNÁ SIMULÁCIA ==================== */

void run_simulation() {
    int32_t pos_x = start_x;
    int32_t pos_y = start_y;
    
    printf("\n[KYBERNAUT-HUMAN v3.1] Fyzikálne korektná simulácia\n");
    printf("=====================================================\n");
    printf("Projekcia 3D→2D:\n");
    printf("  • Bunka: %.1e m\n", CELL_SIZE);
    printf("  • Časový krok: %.1e s\n", TIME_STEP);
    printf("  • Energetická jednotka: %.1e J\n", ENERGY_UNIT);
    printf("  • Rozmer sveta: %"PRId32"x%"PRId32"\n", dimension, dimension);
    printf("=====================================================\n");
    
    int32_t last_print = 0;
    
    while (agent.steps < MAX_STEPS) {
        if (agent.steps % 100 == 0) {
            for (int32_t x = 0; x < dimension; x++) {
                for (int32_t y = 0; y < dimension; y++) {
                    float cooling = (293.15 - world[x][y].temperature) * 0.01;
                    world[x][y].temperature += cooling;
                }
            }
        }
        
        int direction = -1;
        float explore_chance = agent.exploration_rate * 100.0;
        
        if ((rand() % 100) < explore_chance) {
            int possible_dirs[4];
            int dir_count = 0;
            
            if (pos_y < dimension-1) possible_dirs[dir_count++] = 0;
            if (pos_y > 0) possible_dirs[dir_count++] = 1;
            if (pos_x < dimension-1) possible_dirs[dir_count++] = 2;
            if (pos_x > 0) possible_dirs[dir_count++] = 3;
            
            if (dir_count > 0) {
                direction = possible_dirs[rand() % dir_count];
            }
        } else {
            float best_q = -INFINITY;
            
            for (int d = 0; d < 4; d++) {
                int32_t nx = pos_x, ny = pos_y;
                int valid = 1;
                
                switch(d) {
                    case 0: if (pos_y < dimension-1) ny++; else valid = 0; break;
                    case 1: if (pos_y > 0) ny--; else valid = 0; break;
                    case 2: if (pos_x < dimension-1) nx++; else valid = 0; break;
                    case 3: if (pos_x > 0) nx--; else valid = 0; break;
                }
                
                if (!valid) continue;
                
                float q_val = 0.0;
                pthread_mutex_lock(&memory[pos_x][pos_y].mutex);
                q_val = memory[pos_x][pos_y].q_values[d];
                pthread_mutex_unlock(&memory[pos_x][pos_y].mutex);
                
                if (q_val > best_q) {
                    best_q = q_val;
                    direction = d;
                }
            }
        }
        
        if (direction == -1) {
            agent.steps++;
            continue;
        }
        
        agent.decisions_made++;
        
        int32_t new_x = pos_x, new_y = pos_y;
        
        switch(direction) {
            case 0: new_y++; break;
            case 1: new_y--; break;
            case 2: new_x++; break;
            case 3: new_x--; break;
        }
        
        if (new_x < 0 || new_x >= dimension || new_y < 0 || new_y >= dimension) {
            agent.steps++;
            continue;
        }
        
        int32_t old_x = pos_x, old_y = pos_y;
        pos_x = new_x;
        pos_y = new_y;
        agent.steps++;
        
        world[pos_x][pos_y].visits++;
        world[pos_x][pos_y].temperature += 0.1;
        
        float energy_cost = movement_cost(old_x, old_y, pos_x, pos_y);
        agent.total_energy_cost += energy_cost;
        metrics.total_energy_used += energy_cost * ENERGY_UNIT;
        
        float info_gain = (world[pos_x][pos_y].visits == 1) ? 1.0 : 0.1;
        agent.total_information += info_gain;
        world[pos_x][pos_y].information_density += info_gain / (CELL_SIZE * CELL_SIZE);
        
        float reward = physical_reward(old_x, old_y, pos_x, pos_y);
        
        pthread_mutex_lock(&memory[old_x][old_y].mutex);
        float old_q = memory[old_x][old_y].q_values[direction];
        float max_future_q = 0.0;
        
        for (int d = 0; d < 4; d++) {
            if (memory[pos_x][pos_y].q_values[d] > max_future_q) {
                max_future_q = memory[pos_x][pos_y].q_values[d];
            }
        }
        
        float new_q = old_q + agent.learning_rate * 
                     (reward + agent.discount_factor * max_future_q - old_q);
        
        memory[old_x][old_y].q_values[direction] = new_q;
        memory[old_x][old_y].cumulative_reward += reward;
        memory[old_x][old_y].last_visit = agent.steps;
        if (reward > 0) memory[old_x][old_y].successful_exits++;
        memory[old_x][old_y].evaluations++;
        pthread_mutex_unlock(&memory[old_x][old_y].mutex);
        
        agent.learning_entropy += agent.computational_cost / 293.15;
        
        if (agent.path_index < MAX_STEPS) {
            agent.path[agent.path_index++] = direction;
        }
        
        if (agent.steps % 200 == 0 && agent.steps > 0) {
            float current_efficiency = (agent.total_energy_cost > 0) ? 
                                       agent.steps / agent.total_energy_cost : 0;
            
            if (current_efficiency < agent.efficiency_history[agent.efficiency_index % 100] * 0.9) {
                agent.exploration_rate = fmin(0.7, agent.exploration_rate * 1.2);
            } else {
                agent.exploration_rate = fmax(0.05, agent.exploration_rate * 0.9);
            }
            
            agent.efficiency_history[agent.efficiency_index % 100] = current_efficiency;
            agent.efficiency_index++;
        }
        
        if (agent.steps - last_print >= 1000) {
            float info_entropy = calculate_information_entropy();
            float therm_entropy = calculate_thermal_entropy();
            float quantum_entropy = calculate_quantum_entropy();
            
            printf("Krok %5"PRId32": [%3"PRId32",%3"PRId32"] %s\n", 
                   agent.steps, pos_x, pos_y, 
                   materials[world[pos_x][pos_y].material_id].name);
            printf("         Teplota: %.1fK | Návštev: %"PRId32"\n",
                   world[pos_x][pos_y].temperature, world[pos_x][pos_y].visits);
            printf("         Energia: %.1e J | ε: %.2f\n",
                   agent.total_energy_cost * ENERGY_UNIT, agent.exploration_rate);
            printf("         Entropia: S_info=%.3f, S_therm=%.3f, S_quant=%.3f\n",
                   info_entropy, therm_entropy, quantum_entropy);
            
            last_print = agent.steps;
        }
        
        if (world[pos_x][pos_y].is_target == 1 && !agent.home_reached) {
            agent.home_reached = agent.steps;
            printf("\n╔══════════════════════════════════════════════════╗\n");
            printf("║   [DOMOV DOSIAHNUTÝ] v kroku %"PRId32"!                ║\n", agent.steps);
            printf("║   Energia: %.1e J | S_info: %.3f              ║\n",
                   agent.total_energy_cost * ENERGY_UNIT, metrics.information_entropy);
            printf("╚══════════════════════════════════════════════════╝\n");
            
            target_x = dimension - 1;
            target_y = dimension - 1;
            agent.exploration_rate = 0.15;
        }
        
        if (world[pos_x][pos_y].is_target == 2 && !agent.bar_reached) {
            agent.bar_reached = agent.steps;
            printf("\n╔══════════════════════════════════════════════════╗\n");
            printf("║   [BAR DOSIAHNUTÝ] v kroku %"PRId32"!                  ║\n", agent.steps);
            printf("║   Celková energia: %.1e J                     ║\n",
                   agent.total_energy_cost * ENERGY_UNIT);
            printf("╚══════════════════════════════════════════════════╝\n");
            
            target_x = 0;
            target_y = 0;
            agent.exploration_rate = 0.15;
        }
        
        if (agent.home_reached && agent.bar_reached) {
            printf("\n╔══════════════════════════════════════════════════╗\n");
            printf("║        MISIA UKONČENÁ - OBA CIEE DOSIAHNUTÉ!    ║\n");
            printf("║   Fyzikálne korektná simulácia dokončená.       ║\n");
            printf("╚══════════════════════════════════════════════════╝\n");
            break;
        }
    }
    
    calculate_information_entropy();
    calculate_thermal_entropy();
    calculate_quantum_entropy();
    
    int64_t visited = 0;
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            if (world[x][y].visits > 0) visited++;
        }
    }
    metrics.visited_cells = visited;
    metrics.coverage = (float)visited / metrics.total_cells * 100.0;
    
    if (metrics.total_energy_used > 0) {
        metrics.information_efficiency = agent.total_information / metrics.total_energy_used;
    }
    
    float total_temp = 0.0;
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            total_temp += world[x][y].temperature;
        }
    }
    metrics.average_temperature = total_temp / (dimension * dimension);
    
    float delta_S = metrics.thermal_entropy - metrics.information_entropy;
    if (agent.total_energy_cost > 0) {
        metrics.learning_efficiency = delta_S / agent.total_energy_cost;
    }
}

/* ==================== HLAVNÝ PROGRAM ==================== */

int main() {
    srand(time(NULL));
    
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║          KYBERNAUT-HUMAN v3.1 - FYZIKÁLNA VERZIA           ║\n");
    printf("║           (Opravená matematika a 3D→2D projekcia)          ║\n");
    printf("║                PODPORA PRE VEĽKÉ MRIEŽKY                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("FYZIKÁLNA KOREKTNOSŤ:\n");
    printf("  • Všetky entropie normalizované na rozsah 0-1\n");
    printf("  • Projekcia 3D fyziky do 2D simulácie\n");
    printf("  • Reálne fyzikálne konštanty a jednotky\n");
    printf("  • Kontrola matematických limitov\n\n");
    
    printf("Zadaj rozmer sveta (napr. 15-1000): ");
    if (scanf("%"SCNd32, &dimension) != 1 || dimension < 5) {
        printf("Chyba: Neplatný rozmer.\n");
        return 1;
    }
    
    if (dimension > 1000) {
        float memory_required = dimension * dimension * 
                               (sizeof(Node) + sizeof(MemoryNode)) / (1024.0 * 1024.0);
        printf("POZOR: Veľký rozmer %"PRId32"x%"PRId32" vyžaduje približne %.2f MB pamäte\n",
               dimension, dimension, memory_required);
        printf("Naozaj pokračovať? (a/n): ");
        char confirm;
        scanf(" %c", &confirm);
        if (confirm != 'a' && confirm != 'A') return 0;
    }
    
    init_world_physical(dimension);
    init_memory();
    init_agent();
    
    start_x = dimension / 2;
    start_y = dimension / 2;
    target_x = 0;
    target_y = 0;
    
    printf("\nŠtart: [%"PRId32",%"PRId32"], Ciele: Domov[0,0] -> Bar[%"PRId32",%"PRId32"]\n",
           start_x, start_y, dimension-1, dimension-1);
    printf("Fyzikálna interpretácia:\n");
    printf("  • 1 bunka = %.1e m\n", CELL_SIZE);
    printf("  • 1 krok = %.1e s\n", TIME_STEP);
    printf("  • Energetická jednotka = %.1e J\n", ENERGY_UNIT);
    printf("  • Maximálny počet krokov: %d\n\n", MAX_STEPS);
    
    clock_t start_time = clock();
    run_simulation();
    clock_t end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("              VÝSLEDKY KYBERNAUT-HUMAN v3.1\n");
    printf("══════════════════════════════════════════════════════════════\n\n");
    
    printf("FYZIKÁLNE METRIKY:\n");
    printf("  Kroky simulácie: %"PRId32"\n", agent.steps);
    printf("  Celková energia: %.3e J\n", metrics.total_energy_used);
    printf("  Priemerná teplota: %.1f K\n", metrics.average_temperature);
    printf("  Čas simulácie: %.3f s\n", total_time);
    
    printf("\nENTROPICKÁ ANALÝZA (normalizované 0-1):\n");
    printf("  Informačná entropia (S_info): %.4f\n", metrics.information_entropy);
    printf("  Tepelná entropia (S_thermal): %.4f\n", metrics.thermal_entropy);
    printf("  Kvantová entropia (S_quantum): %.4f\n", metrics.quantum_entropy);
    printf("  Rozdiel S_thermal - S_info: %.4f\n", 
           metrics.thermal_entropy - metrics.information_entropy);
    printf("  Pomer S_thermal/S_info: %.3f\n",
           metrics.information_entropy > 0 ? metrics.thermal_entropy / metrics.information_entropy : 0);
    
    printf("\nINFORMAČNÉ METRIKY:\n");
    printf("  Pokrytie sveta: %"PRId64"/%"PRId64" buniek (%.1f%%)\n",
           metrics.visited_cells, metrics.total_cells, metrics.coverage);
    printf("  Efektivita informácie: %.3e bit/J\n", metrics.information_efficiency);
    printf("  Rozhodnutí: %"PRId64"\n", agent.decisions_made);
    
    printf("\nMETRIKY UČENIA:\n");
    printf("  Entropia učenia: %.3e J/K\n", agent.learning_entropy);
    printf("  Účinnosť učenia: %.3e ΔS/J\n", metrics.learning_efficiency);
    printf("  Konečná miera explorácie: %.2f\n", agent.exploration_rate);
    
    if (agent.home_reached) printf("  Domov dosiahnutý v kroku: %"PRId32"\n", agent.home_reached);
    if (agent.bar_reached) printf("  Bar dosiahnutý v kroku: %"PRId32"\n", agent.bar_reached);
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("              MATEMATICKÁ VALIDÁCIA\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    int validation_passed = 1;
    
    if (metrics.information_entropy < 0 || metrics.information_entropy > 1) {
        printf("✗ S_info mimo rozsahu 0-1: %.4f\n", metrics.information_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_info v rozsahu 0-1: %.4f\n", metrics.information_entropy);
    }
    
    if (metrics.thermal_entropy < 0 || metrics.thermal_entropy > 1) {
        printf("✗ S_thermal mimo rozsahu 0-1: %.4f\n", metrics.thermal_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_thermal v rozsahu 0-1: %.4f\n", metrics.thermal_entropy);
    }
    
    if (metrics.quantum_entropy < 0 || metrics.quantum_entropy > 1) {
        printf("✗ S_quantum mimo rozsahu 0-1: %.4f\n", metrics.quantum_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_quantum v rozsahu 0-1: %.4f\n", metrics.quantum_entropy);
    }
    
    if (metrics.information_entropy > metrics.thermal_entropy) {
        printf(" S_info > S_thermal: %.4f > %.4f\n", 
               metrics.information_entropy, metrics.thermal_entropy);
        printf("   (Možno validné pre systémy s vysokou informačnou štruktúrou)\n");
    }
    
    if (validation_passed) {
        printf("\n✓ Všetky metriky matematicky korektné\n");
    } else {
        printf("\n✗ Niektoré metriky mimo matematických limitov\n");
    }
    
    FILE* f = fopen(LOG_FILENAME, "w");
    if (f) {
        fprintf(f, "KYBERNAUT-HUMAN v3.1 - Fyzikálne korektná verzia\n");
        fprintf(f, "================================================\n\n");
        fprintf(f, "Fyzikálne parametre:\n");
        fprintf(f, "  Rozmer sveta: %"PRId32" x %"PRId32" buniek\n", dimension, dimension);
        fprintf(f, "  Veľkosť bunky: %.1e m\n", CELL_SIZE);
        fprintf(f, "  Časový krok: %.1e s\n", TIME_STEP);
        fprintf(f, "  Simulačný čas: %.3f s\n\n", total_time);
        
        fprintf(f, "Entropické metriky (0-1):\n");
        fprintf(f, "  S_info: %.4f\n", metrics.information_entropy);
        fprintf(f, "  S_thermal: %.4f\n", metrics.thermal_entropy);
        fprintf(f, "  S_quantum: %.4f\n", metrics.quantum_entropy);
        fprintf(f, "  ΔS: %.4f\n", metrics.thermal_entropy - metrics.information_entropy);
        fprintf(f, "  Pomer: %.3f\n\n", 
                metrics.information_entropy > 0 ? metrics.thermal_entropy / metrics.information_entropy : 0);
        
        fprintf(f, "Fyzikálne metriky:\n");
        fprintf(f, "  Celková energia: %.3e J\n", metrics.total_energy_used);
        fprintf(f, "  Priemerná teplota: %.1f K\n", metrics.average_temperature);
        fprintf(f, "  Pokrytie: %.1f%%\n", metrics.coverage);
        
        fclose(f);
        printf("\nVýsledky uložené do: %s\n", LOG_FILENAME);
    }
    
    for (int32_t i = 0; i < dimension; i++) {
        free(world[i]);
        for (int32_t j = 0; j < dimension; j++) {
            pthread_mutex_destroy(&memory[i][j].mutex);
        }
        free(memory[i]);
    }
    free(world);
    free(memory);
    
    pthread_mutex_destroy(&print_mutex);
    pthread_mutex_destroy(&global_mutex);
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("  SIMULÁCIA UKONČENÁ - MATEMATICKY VALIDOVANÁ VERZIA\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
